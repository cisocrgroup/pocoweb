#include "LineRoute.hpp"
#include "core/Pix.hpp"
#include "core/WagnerFischer.hpp"
#include "core/jsonify.hpp"
#include "core/util.hpp"
#include "database/DbStructs.hpp"
#include "utils/Error.hpp"
#include <basen.hpp>
#include <crow.h>

#define LINE_ROUTE_ROUTE "/books/<int>/pages/<int>/lines/<int>"
#define WORD_ROUTE_ROUTE "/books/<int>/pages/<int>/lines/<int>/tokens/<int>"

using lock_set = std::set<std::tuple<int, int, int>>;
static std::mutex LOCK;
static lock_set ID_SET;

struct line_lock_guard {
  line_lock_guard(int bid, int pid, int lid, std::mutex &lock,
                  lock_set &id_locks);
  ~line_lock_guard();
  std::tuple<int, int, int> t_;
  std::mutex &lock_;
  lock_set &id_locks_;
};

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
const char *LineRoute::route_ = LINE_ROUTE_ROUTE "," WORD_ROUTE_ROUTE;
const char *LineRoute::name_ = "LineRoute";

////////////////////////////////////////////////////////////////////////////////
void LineRoute::Register(App &app) {
  CROW_ROUTE(app, LINE_ROUTE_ROUTE)
      .methods("GET"_method, "PUT"_method, "POST"_method,
               "DELETE"_method)(*this);
  CROW_ROUTE(app, WORD_ROUTE_ROUTE)
      .methods("GET"_method, "PUT"_method, "POST"_method)(*this);
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
// PUT /books/<bid>/pages/<pid>/lines/<lid>
////////////////////////////////////////////////////////////////////////////////
Route::Response LineRoute::impl(HttpPut, const Request &req, int pid, int p,
                                int lid) const {
  CROW_LOG_INFO << "(LineRoute) PUT line: " << pid << ":" << p << ":" << lid;
  return impl(HttpPost{}, req, pid, p, lid);
}

////////////////////////////////////////////////////////////////////////////////
// POST /books/<bid>/pages/<pid>/lines/<lid>?t=...
////////////////////////////////////////////////////////////////////////////////
Route::Response LineRoute::impl(HttpPost, const Request &req, int pid, int p,
                                int lid) const {
  const auto t = get<std::string>(req.url_params, "t").value_or("automatic");
  CROW_LOG_INFO << "(LineRoute) POST line: " << pid << ":" << p << ":" << lid
                << " t = " << t;
  auto conn = must_get_connection();
  line_lock_guard lock(pid, p, lid, LOCK, ID_SET);
  DbLine line(pid, p, lid);
  if (not line.load(conn)) {
    THROW(NotFound, "(LineRoute) cannot find ", pid, ":", p, ":", lid);
  }
  auto slice = line.slice();
  if (t == "ocr") {
    updateOCR(req, line);
  } else if (t == "manual" or t == "automatic") {
    correct(req, slice, t == "manual");
  } else if (t == "reset") {
    reset(req, slice);
  } else {
    THROW(BadRequest, "(LineRoute) invalid type: ", t);
  }
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
  return j << line.slice(tid, len.value_or(-1));
}

////////////////////////////////////////////////////////////////////////////////
// PUT /books/<bid>/pages/<pid>/lines/<lid>/tokens/<tid>{?len=}
////////////////////////////////////////////////////////////////////////////////
Route::Response LineRoute::impl(HttpPut, const Request &req, int pid, int p,
                                int lid, int tid) const {
  CROW_LOG_INFO << "(LineRoute) PUT token: " << pid << ":" << p << ":" << lid
                << ":" << tid;
  return impl(HttpPost{}, req, pid, p, lid, tid);
}

////////////////////////////////////////////////////////////////////////////////
// POST /books/<bid>/pages/<pid>/lines/<lid>/tokens/<tid>{?len=}
////////////////////////////////////////////////////////////////////////////////
Route::Response LineRoute::impl(HttpPost, const Request &req, int pid, int p,
                                int lid, int tid) const {
  const auto t = get<std::string>(req.url_params, "t").value_or("automatic");
  const auto len = get<int>(req.url_params, "len").value_or(-1);
  CROW_LOG_INFO << "(LineRoute) POST token: " << pid << ":" << p << ":" << lid
                << ":" << tid << " t = " << t << " len = " << len;
  auto conn = must_get_connection();
  line_lock_guard lock(pid, p, lid, LOCK, ID_SET);
  DbLine line(pid, p, lid);
  if (not line.load(conn)) {
    THROW(NotFound, "(LineRoute) cannot find ", pid, ":", p, ":", lid);
  }
  auto slice = line.slice(tid, len);
  if (t == "manual" or t == "automatic") {
    correct(req, slice, t == "manual");
  } else if (t == "reset") {
    reset(req, slice);
  } else {
    THROW(BadRequest, "(LineRoute) invalid type: ", t);
  }
  update(conn, line);
  Json j;
  return j << slice;
}

////////////////////////////////////////////////////////////////////////////////
void LineRoute::updateOCR(const Request &req, DbLine &line) const {
  CROW_LOG_INFO << "(LineRoute::updateOCR) line: " << line.strID();
  const auto json = crow::json::load(req.body);
  const auto imagedata = get<std::string>(json, "imgData").value_or("");
  const auto ocr = utf8(get<std::string>(json, "ocr").value_or(""));
  const auto cuts =
      get<std::vector<int>>(json, "cuts").value_or(std::vector<int>{});
  const auto confs = get<std::vector<double>>(json, "confidences")
                         .value_or(std::vector<double>{});
  if (ocr.size() != cuts.size()) {
    THROW(BadRequest,
          "(LineRoute::updateOCR) len of cuts and ocr do not coincide");
  }
  line.updateOCR(ocr, cuts, confs);
  if (imagedata != "") {
    CROW_LOG_INFO << "(LineRoute::updateOCR) updating image data for line: "
                  << line.strID();
    updateImage(imagedata, line);
  }
}

////////////////////////////////////////////////////////////////////////////////
void LineRoute::updateImage(const std::string &data, DbLine &line) const {
  const auto path =
      fs::path(get_config().daemon.projectdir).parent_path() / line.imagepath;
  CROW_LOG_INFO << "(LineRoute::updateImage) updating image data for line: "
                << line.strID() << ": " << path;
  std::ofstream out(path.string());
  if (not out.is_open()) {
    THROW(Error, "(LineRoute::updateImage) cannot open file: ", path.string());
  }
  bn::decode_b64(data.begin(), data.end(),
                 std::ostream_iterator<char>(out, ""));
  out.close();
  // get image dimensions
  PixPtr pix;
  pix.reset(pixRead(path.string().data()));
  if (not pix) {
    THROW(Error, "(LineRoute::udpateImage) cannot read image: ", path.string());
  }
  int ec, w, h;
  if ((ec = pixGetDimensions(pix.get(), &w, &h, nullptr))) {
    THROW(Error, "(LineRoute::udpateImage) cannot get image dims: %d", ec);
  }
  // update image bounding box
  line.box.set_left(0);
  line.box.set_right(w);
  line.box.set_top(0);
  line.box.set_bottom(h);
}

////////////////////////////////////////////////////////////////////////////////
void LineRoute::correct(const Request &req, DbSlice &slice, bool manual) {
  auto cor = get<std::string>(crow::json::load(req.body), "correction");
  if (not cor) {
    THROW(BadRequest, "(LineRoute::correct) missing correction data");
  }
  WagnerFischer wf;
  wf.set_gt(cor.value());
  // can only overwrite manual correctinos with other manual corrections
  if (not manual and slice.contains_manual_corrections()) {
    THROW(Conflict, "(LineRoute::correct) cannot overwrite manual corrections");
  }
  wf.set_ocr(slice.wocr());
  const auto lev = wf();
  CROW_LOG_DEBUG << "(LineRoute::correct) correction: " << cor.value();
  CROW_LOG_DEBUG << "(LineRoute::correct) line.ocr(): " << slice.ocr();
  CROW_LOG_DEBUG << "(LineRoute::correct) line.cor(): " << slice.cor();

  // correct
  wf.correct(slice);
  slice.set_correction_type(manual);

  CROW_LOG_DEBUG << "(LineRoute::correct) line.ocr(): " << slice.ocr();
  CROW_LOG_DEBUG << "(LineRoute::correct) line.cor(): " << slice.cor();
  CROW_LOG_DEBUG << "(LineRoute::correct)        lev: " << lev;
}

////////////////////////////////////////////////////////////////////////////////
void LineRoute::reset(const Request &req, DbSlice &slice) {
  CROW_LOG_DEBUG << "(LineRoute::reset) cor: " << slice.cor();
  CROW_LOG_DEBUG << "(LineRoute::reset) ocr: " << slice.ocr();
  slice.reset();
  slice.set_correction_type(false);
  CROW_LOG_DEBUG << "(LineRoute::reset) cor: " << slice.cor();
}

////////////////////////////////////////////////////////////////////////////////
void LineRoute::update(MysqlConnection &mysql, const DbLine &line) {
  using namespace sqlpp;
  tables::Contents contents;
  tables::Textlines lines;
  MysqlCommitter committer(mysql);
  // update image
  mysql.db()(
      sqlpp::update(lines)
          .set(lines.lleft = line.box.left(), lines.lright = line.box.right(),
               lines.ltop = line.box.top(), lines.lbottom = line.box.bottom())
          .where(lines.bookid == line.bookid and lines.pageid == line.pageid and
                 lines.lineid == line.lineid));
  // delete old contents
  mysql.db()(remove_from(contents).where(contents.bookid == line.bookid and
                                         contents.pageid == line.pageid and
                                         contents.lineid == line.lineid));
  // add new contents
  auto stmnt = mysql.db().prepare(insert_into(contents).set(
      contents.bookid = line.bookid, contents.pageid = line.pageid,
      contents.lineid = line.lineid, contents.seq = parameter(contents.seq),
      contents.cid = parameter(contents.cid),
      contents.ocr = parameter(contents.ocr),
      contents.cor = parameter(contents.cor),
      contents.cut = parameter(contents.cut),
      contents.manually = parameter(contents.manually),
      contents.conf = parameter(contents.conf)));
  int i = 0;
  for (const auto &c : line.line) {
    stmnt.params.seq = i++;
    stmnt.params.ocr = int(c.ocr);
    stmnt.params.cor = int(c.cor);
    stmnt.params.cut = c.cut;
    stmnt.params.manually = c.manually;
    stmnt.params.conf = c.conf;
    stmnt.params.cid = c.id;
    mysql.db()(stmnt);
  }
  committer.commit();
}

////////////////////////////////////////////////////////////////////////////////
line_lock_guard::line_lock_guard(int bid, int pid, int lid, std::mutex &lock,
                                 lock_set &id_locks)
    : t_(std::make_tuple(bid, pid, lid)), lock_(lock), id_locks_(id_locks) {
  /* spin lock */
  while (true) {
    std::lock_guard<std::mutex> lock(lock_);
    if (not id_locks_.count(t_)) {
      id_locks_.insert(t_);
      break;
    }
  }
}

////////////////////////////////////////////////////////////////////////////////
line_lock_guard::~line_lock_guard() {
  std::lock_guard<std::mutex> lock(lock_);
  auto i = id_locks_.find(t_);
  assert(i != id_locks_.end());
  id_locks_.erase(i);
}
