#include "LineRoute.hpp"
#include "core/Book.hpp"
#include "core/BookDirectoryBuilder.hpp"
#include "core/Package.hpp"
#include "core/Page.hpp"
#include "core/Session.hpp"
#include "core/SessionDirectory.hpp"
#include "core/WagnerFischer.hpp"
#include "core/jsonify.hpp"
#include "utils/Error.hpp"
#include "utils/ScopeGuard.hpp"
#include <crow.h>
#include <regex>
#include <unicode/uchar.h>
#include <utf8.h>

#define LINE_ROUTE_ROUTE "/books/<int>/pages/<int>/lines/<int>"
#define WORD_ROUTE_ROUTE "/books/<int>/pages/<int>/lines/<int>/tokens/<int>"

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
const char *LineRoute::route_ = LINE_ROUTE_ROUTE "," WORD_ROUTE_ROUTE;
const char *LineRoute::name_ = "LineRoute";

////////////////////////////////////////////////////////////////////////////////
void LineRoute::Register(App &app) {
  CROW_ROUTE(app, LINE_ROUTE_ROUTE)
      .methods("GET"_method, "POST"_method, "DELETE"_method)(*this);
  CROW_ROUTE(app, WORD_ROUTE_ROUTE).methods("GET"_method, "POST"_method)(*this);
}

////////////////////////////////////////////////////////////////////////////////
Route::Response LineRoute::impl(HttpGet, const Request &req, int bid, int pid,
                                int lid) const {
  LockedSession session(get_session(req));
  auto conn = must_get_connection();
  auto line = session->must_find(conn, bid, pid, lid);
  Json j;
  return wj(j, *line, bid);
}

////////////////////////////////////////////////////////////////////////////////
Route::Response LineRoute::impl(HttpPost, const Request &req, int pid, int p,
                                int lid) const {
  CROW_LOG_INFO << "POST line: " << pid << ":" << p << ":" << lid;
  const auto json = crow::json::load(req.body);
  const auto correction = get<std::string>(json, "correction");
  if (not correction) {
    THROW(BadRequest, "(LineRoute) missing correction data");
  }
  LockedSession session(get_session(req));
  auto conn = must_get_connection();
  Corrector corrector(conn, pid, p, lid);
  return corrector.toJSON(corrector.correctWholeLine(correction.value()));
}

////////////////////////////////////////////////////////////////////////////////
Route::Response LineRoute::impl(HttpDelete, const Request &req, int pid, int p,
                                int lid) const {
  LockedSession session(get_session(req));
  auto conn = must_get_connection();
  auto page = session->must_find(conn, pid, p);
  if (!page->book().is_book()) {
    THROW(BadRequest,
          "(LineRoute) cannot delete line from project (must be a book)");
  }
  MysqlCommitter committer(conn);
  delete_line(conn.db(), pid, p, lid);
  page->delete_line(lid);
  committer.commit();
  return ok();
}

////////////////////////////////////////////////////////////////////////////////
Route::Response LineRoute::impl(HttpGet, const Request &req, int pid, int p,
                                int lid, int tid) const {
  LockedSession session(get_session(req));
  auto conn = must_get_connection();
  auto line = session->must_find(conn, pid, p, lid);
  auto token = find_token(*line, tid);
  if (not token) {
    THROW(NotFound, "(LineRoute) cannot find token id ", tid);
  }
  Json j;
  return wj(j, *token, pid);
}

////////////////////////////////////////////////////////////////////////////////
Route::Response LineRoute::impl(HttpPost, const Request &req, int pid, int p,
                                int lid, int tid) const {
  const auto json = crow::json::load(req.body);
  const auto c = get<std::string>(json, "correction");
  if (not c) {
    THROW(BadRequest, "(LineRoute) missing correction data");
  }
  LockedSession session(get_session(req));
  auto conn = must_get_connection();
  auto line = session->must_find(conn, pid, p, lid);
  return correct(conn, pid, tid, *line, *c);
}

////////////////////////////////////////////////////////////////////////////////
Route::Response LineRoute::correct(MysqlConnection &conn, int pid, int p,
                                   Line &line, const std::string &c) const {
  WagnerFischer wf;
  wf.set_gt(c);
  wf.set_ocr(line);
  const auto lev = wf();
  CROW_LOG_DEBUG << "(LineRoute) correction: " << c;
  CROW_LOG_DEBUG << "(LineRoute) line.cor(): " << line.cor();
  CROW_LOG_DEBUG << "(LineRoute) line.ocr(): " << line.ocr();
  CROW_LOG_DEBUG << "(LineRoute)        lev: " << lev;
  wf.correct(line);
  using namespace sqlpp;
  tables::Contents contents;
  conn.db()(remove_from(contents).where(contents.bookid == pid and
                                        contents.pageid == p and
                                        contents.lineid == line.id()));
  auto insert = conn.db().prepare(insert_into(contents).set(
      contents.bookid = pid, contents.pageid = p, contents.lineid = line.id(),
      contents.seq = parameter(contents.seq),
      contents.ocr = parameter(contents.ocr),
      contents.cor = parameter(contents.cor),
      contents.cut = parameter(contents.cut),
      contents.conf = parameter(contents.conf)));

  for (auto i = 0U; i < line.size(); i++) {
    insert.params.seq = i;
    insert.params.ocr = line[i].ocr;
    insert.params.cor = line[i].cor;
    insert.params.cut = line[i].cut;
    insert.params.conf = line[i].conf;
    conn.db()(insert);
  }
  Json j;
  return wj(j, line, pid);
}

////////////////////////////////////////////////////////////////////////////////
Route::Response LineRoute::correctx(MysqlConnection &conn, int pid, int tid,
                                    Line &line, const std::string &c) const {
  auto token = find_token(line, tid);
  if (not token) {
    THROW(NotFound, "(LineRoute) cannot find ", line.page().book().id(), "-",
          line.page().id(), "-", line.id(), "-", tid);
  }
  WagnerFischer wf;
  wf.set_ocr(line);
  wf.set_gt(c);
  const auto b = token->offset();
  const auto n = token->size();
  if (b <= 0 || b >= line.size() || (b + n) > line.size()) {
    THROW(Error, "invalid token offset: ", b, ", ", n);
  }
  const auto lev = wf(b, n);
  CROW_LOG_DEBUG << "(LineRoute) correction: " << c;
  CROW_LOG_DEBUG << "(LineRoute) line.cor(): " << line.cor();
  CROW_LOG_DEBUG << "(LineRoute) line.ocr(): " << line.ocr();
  CROW_LOG_DEBUG << "(LineRoute)        lev: " << lev;
  wf.correct(line, b, n);
  token = find_token(line, tid);
  if (not token) {
    THROW(Error, "(LineRoute) cannot find token ", line.page().book().id(), "-",
          line.page().id(), "-", line.id(), "-", tid, " after correction");
  }
  update_line(conn, line);
  Json j;
  return wj(j, *token, pid);
}

////////////////////////////////////////////////////////////////////////////////
boost::optional<Token> LineRoute::find_token(const Line &line, int tid) {
  boost::optional<Token> token;
  line.each_token([&](const auto &t) {
    if (t.id == tid) {
      token = t;
    }
  });
  return token;
}

////////////////////////////////////////////////////////////////////////////////
void LineRoute::update_line(MysqlConnection &conn, const Line &line) {
  MysqlCommitter committer(conn);
  pcw::update_line(conn.db(), line);
  committer.commit();
}
////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<Line>
Corrector::correctWholeLine(const std::string &correction) {
  MysqlCommitter committer(c_);
  origin_ = loadOrigin();
  assert(origin_ != 0);
  auto line = loadLine();
  assert(line);
  correct(*line, correction);
  committer.commit();
  return line;
}

////////////////////////////////////////////////////////////////////////////////
void Corrector::correct(Line &line, const std::string &correction) const {
  WagnerFischer wf;
  wf.set_gt(correction);
  wf.set_ocr(line);
  const auto lev = wf();
  CROW_LOG_INFO << "(Corrector) correction: " << correction;
  CROW_LOG_INFO << "(Corrector) line.cor(): " << line.cor();
  CROW_LOG_INFO << "(Corrector) line.ocr(): " << line.ocr();
  CROW_LOG_INFO << "(Corrector)        lev: " << lev;
  wf.correct(line);
  using namespace sqlpp;
  tables::Contents c;
  // delete all line content
  c_.db()(remove_from(c).where(c.bookid == origin_ and c.pageid == pageid_ and
                               c.lineid == lid_));
  // insert corrected line
  auto insert = c_.db().prepare(
      insert_into(c).set(c.bookid = origin_, c.pageid = pageid_,
                         c.lineid = lid_, c.seq = parameter(c.seq),
                         c.ocr = parameter(c.ocr), c.cor = parameter(c.cor),
                         c.cut = parameter(c.cut), c.conf = parameter(c.conf)));
  for (auto i = 0U; i < line.size(); i++) {
    insert.params.seq = i;
    insert.params.ocr = line[i].ocr;
    insert.params.cor = line[i].cor;
    insert.params.cut = line[i].cut;
    insert.params.conf = line[i].conf;
    c_.db()(insert);
  }
}

////////////////////////////////////////////////////////////////////////////////
int Corrector::loadOrigin() const {
  using namespace sqlpp;
  tables::Projects p;
  auto rows = c_.db()(select(p.origin).from(p).where(p.id == pid_));
  if (rows.empty()) {
    THROW(NotFound, "(Corrector) cannot find origin id for project");
  }
  return rows.front().origin;
}

////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<Line> Corrector::loadLine() const {
  using namespace sqlpp;
  tables::Contents c;
  tables::Textlines l;
  auto rows = c_.db()(
      select(all_of(c), l.imagepath, l.lleft, l.lright, l.ltop, l.lbottom)
          .from(c.join(l).on(c.bookid == l.bookid))
          .where(c.lineid == lid_ and c.pageid == pageid_ and
                 c.bookid == origin_)
          .order_by(c.seq.asc()));
  if (rows.empty()) {
    THROW(NotFound, "(Corrector) no content");
  }
  LineBuilder builder;
  builder.set_id(lid_);
  builder.set_image_path(static_cast<std::string>(rows.front().imagepath));
  builder.set_box({static_cast<int>(rows.front().lleft),
                   static_cast<int>(rows.front().ltop),
                   static_cast<int>(rows.front().lright),
                   static_cast<int>(rows.front().lbottom)});
  for (const auto &row : rows) {
    builder.append(static_cast<wchar_t>(row.ocr), static_cast<wchar_t>(row.cor),
                   row.cut, row.conf);
  }
  return builder.build();
}

////////////////////////////////////////////////////////////////////////////////
Json Corrector::toJSON(const std::shared_ptr<Line> &line) const {
  assert(line);
  Json j;
  j["lineId"] = lid_;
  j["pageId"] = pageid_;
  j["projectId"] = pid_;
  j["bookId"] = origin_;
  j["box"] << line->box;
  j["imgFile"] = line->img.native();
  j["cor"] = line->cor();
  j["ocr"] = line->ocr();
  j["cuts"] = line->cuts();
  j["confidences"] = line->confidences();
  j["averageConfidence"] = fix_double(line->average_conf());
  j["isFullyCorrected"] = line->is_fully_corrected();
  j["isPartiallyCorrected"] = line->is_partially_corrected();
  size_t i = 0;
  line->each_token([&](const auto &token) {
    j["tokens"][i]["projectId"] = pid_;
    j["tokens"][i]["bookId"] = origin_;
    j["tokens"][i]["pageId"] = pageid_;
    j["tokens"][i]["lineId"] = lid_;
    j["tokens"][i]["offset"] = token.offset();
    j["tokens"][i]["tokenId"] = token.id;
    j["tokens"][i]["isFullyCorrected"] = token.is_fully_corrected();
    j["tokens"][i]["isPartiallyCorrected"] = token.is_partially_corrected();
    j["tokens"][i]["ocr"] = token.ocr();
    j["tokens"][i]["cor"] = token.cor();
    j["tokens"][i]["averageConfidence"] = fix_double(token.average_conf());
    j["tokens"][i]["box"] << token.box;
    j["tokens"][i]["isNormal"] = token.is_normal();
    ++i;
  });
  return j;
}
