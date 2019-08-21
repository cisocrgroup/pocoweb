#include "LineRoute.hpp"
#include "core/WagnerFischer.hpp"
#include "core/jsonify.hpp"
#include "database/DbStructs.hpp"
#include "utils/Error.hpp"
#include <crow.h>

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
// GET /books/<bid>/pages/<pid>/lines/<lid>
////////////////////////////////////////////////////////////////////////////////
Route::Response LineRoute::impl(HttpGet, const Request &req, int pid,
                                int pageid, int lid) const {
  CROW_LOG_INFO << "(LineRoute) GET line: " << pid << ":" << pageid << ":"
                << lid;
  auto conn = must_get_connection();
  DbLine line(pid, pageid, lid);
  if (not line.load(conn)) {
    THROW(NotFound, "(LineRoute) cannot find ", pid, ":", pageid, ":", lid);
  }
  Json j;
  return j << line;
}

////////////////////////////////////////////////////////////////////////////////
// POST /books/<bid>/pages/<pid>/lines/<lid>
////////////////////////////////////////////////////////////////////////////////
Route::Response LineRoute::impl(HttpPost, const Request &req, int pid, int p,
                                int lid) const {
  CROW_LOG_INFO << "(LineRoute) POST line: " << pid << ":" << p << ":" << lid;
  const auto json = crow::json::load(req.body);
  const auto correction = get<std::string>(json, "correction");
  if (not correction) {
    THROW(BadRequest, "(LineRoute) missing correction data");
  }
  DbLine line(pid, p, lid);
  auto conn = must_get_connection();
  if (not line.load(conn)) {
    THROW(NotFound, "(LineRoute) cannot find ", pid, ":", p, ":", lid);
  }
  correct(line, correction.value());
  update(conn, line);
  Json j;
  return j << line;
}

////////////////////////////////////////////////////////////////////////////////
// DELETE /books/<bid>/pages/<pid>/lines/<lid>
////////////////////////////////////////////////////////////////////////////////
Route::Response LineRoute::impl(HttpDelete, const Request &req, int pid, int p,
                                int lid) const {
  CROW_LOG_INFO << "(LineRoute) DELETE line: " << pid << ":" << p << ":" << lid;
  auto conn = must_get_connection();
  DbLine line(pid, p, lid);
  if (not line.load(conn)) {
    THROW(NotFound, "(LineRoute) cannot find ", pid, ":", p, ":", lid);
  }
  using namespace sqlpp;
  tables::Contents contents;
  tables::Textlines lines;
  MysqlCommitter committer(conn);
  // delete contents
  conn.db()(remove_from(contents).where(contents.bookid == line.bookid and
                                        contents.pageid == line.pageid and
                                        contents.lineid == line.lineid));
  // delete textline
  conn.db()(remove_from(lines).where(lines.bookid == line.bookid and
                                     lines.pageid == line.pageid and
                                     lines.lineid == line.lineid));
  committer.commit();
  return ok();
}

////////////////////////////////////////////////////////////////////////////////
// GET /books/<bid>/pages/<pid>/lines/<lid>/tokens/<tid>{?len=}
////////////////////////////////////////////////////////////////////////////////
Route::Response LineRoute::impl(HttpGet, const Request &req, int pid,
                                int pageid, int lid, int tid) const {
  CROW_LOG_INFO << "(LineRoute) GET token: " << pid << ":" << pageid << ":"
                << lid << ":" << tid;
  const auto len = get<int>(req.url_params, "len");
  auto conn = must_get_connection();
  DbLine line{pid, pageid, lid};
  if (not line.load(conn)) {
    THROW(NotFound, "(LineRoute) cannot find ", pid, ":", pageid, ":", lid);
  }
  Json j;
  if (len) {
    return j << line.slice(tid, len.value());
  }
  return j << line.slice(tid, line.tokenLength(tid));
}

////////////////////////////////////////////////////////////////////////////////
// POST /books/<bid>/pages/<pid>/lines/<lid>/tokens/<tid>{?len=}
////////////////////////////////////////////////////////////////////////////////
Route::Response LineRoute::impl(HttpPost, const Request &req, int pid, int p,
                                int lid, int tid) const {
  CROW_LOG_INFO << "(LineRoute) POST token: " << pid << ":" << p << ":" << lid
                << ":" << tid;
  const auto len = get<int>(req.url_params, "len");
  const auto json = crow::json::load(req.body);
  const auto correction = get<std::string>(json, "correction");
  if (not correction) {
    THROW(BadRequest, "(LineRoute) missing correction data");
  }
  auto conn = must_get_connection();
  DbLine line{pid, p, lid};
  if (not line.load(conn)) {
    THROW(NotFound, "(LineRoute) cannot find ", pid, ":", p, ":", lid);
  }
  const auto l = len ? len.value() : line.tokenLength(tid);
  correct(line, correction.value(), tid, l);
  update(conn, line);
  Json j;
  return j << line.slice(tid, int(correction.value().size()));
}

////////////////////////////////////////////////////////////////////////////////
void LineRoute::correct(DbLine &line, const std::string &correction) {
  WagnerFischer wf;
  const auto slice = line.slice();
  wf.set_gt(correction);
  wf.set_ocr(slice.ocr());
  const auto lev = wf();
  CROW_LOG_INFO << "(LineRoute) correction: " << correction;
  CROW_LOG_INFO << "(LineRoute) line.ocr(): " << slice.ocr();

  // correct
  wf.correct(line);

  CROW_LOG_INFO << "(LineRoute) line.cor(): " << slice.cor();
  CROW_LOG_INFO << "(LineRoute)        lev: " << lev;
}

////////////////////////////////////////////////////////////////////////////////
void LineRoute::correct(DbLine &line, const std::string &correction, int b,
                        int len) {
  WagnerFischer wf;
  const auto slice = line.slice();
  wf.set_gt(correction);
  wf.set_ocr(slice.ocr());
  const auto lev = wf(b, len);
  CROW_LOG_INFO << "(LineRoute) correction: " << correction;
  CROW_LOG_INFO << "(LineRoute) line.ocr(): " << slice.ocr();

  // correct
  wf.correct(line, b, len);

  CROW_LOG_INFO << "(LineRoute) line.cor(): " << slice.cor();
  CROW_LOG_INFO << "(LineRoute)        lev: " << lev;
}

////////////////////////////////////////////////////////////////////////////////
void LineRoute::update(MysqlConnection &mysql, const DbLine &line) {
  using namespace sqlpp;
  tables::Contents contents;
  MysqlCommitter committer(mysql);
  // delete old contents
  mysql.db()(remove_from(contents).where(contents.bookid == line.bookid and
                                         contents.pageid == line.pageid and
                                         contents.lineid == line.lineid));
  // add new contents
  auto stmnt = mysql.db().prepare(insert_into(contents).set(
      contents.bookid = line.bookid, contents.pageid = line.pageid,
      contents.lineid = line.lineid, contents.seq = parameter(contents.seq),
      contents.ocr = parameter(contents.ocr),
      contents.cor = parameter(contents.cor),
      contents.cut = parameter(contents.cut),
      contents.conf = parameter(contents.conf)));
  for (auto i = 0U; i < line.line.size(); i++) {
    stmnt.params.seq = i;
    stmnt.params.ocr = int(line.line[i].ocr);
    stmnt.params.cor = int(line.line[i].cor);
    stmnt.params.cut = line.line[i].cut;
    stmnt.params.conf = line.line[i].conf;
    mysql.db()(stmnt);
  }
  committer.commit();
}
