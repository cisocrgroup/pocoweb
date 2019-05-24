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
const char* LineRoute::route_ = LINE_ROUTE_ROUTE "," WORD_ROUTE_ROUTE;
const char* LineRoute::name_ = "LineRoute";

////////////////////////////////////////////////////////////////////////////////
void
LineRoute::Register(App& app)
{
  CROW_ROUTE(app, LINE_ROUTE_ROUTE)
    .methods("GET"_method, "POST"_method, "DELETE"_method)(*this);
  CROW_ROUTE(app, WORD_ROUTE_ROUTE).methods("GET"_method, "POST"_method)(*this);
}

////////////////////////////////////////////////////////////////////////////////
Route::Response
LineRoute::impl(HttpGet, const Request& req, int bid, int pid, int lid) const
{
  LockedSession session(get_session(req));
  auto conn = must_get_connection();
  CROW_LOG_DEBUG << "get line " << bid << " " << pid << " " << lid;
  auto line = session->must_find(conn, bid, pid, lid);
  CROW_LOG_DEBUG << "found line: " << line->page().book().id() << " "
                 << line->page().book().origin().id() << " "
                 << line->page().id() << " " << line->id();
  Json j;
  return j << *line;
}

////////////////////////////////////////////////////////////////////////////////
Route::Response
LineRoute::impl(HttpPost, const Request& req, int pid, int p, int lid) const
{
  const auto json = crow::json::load(req.body);
  const auto c = get<std::string>(json, "correction");
  if (not c)
    THROW(BadRequest, "(LineRoute) missing correction data");
  LockedSession session(get_session(req));
  auto conn = must_get_connection();
  auto line = session->must_find(conn, pid, p, lid);
  return correct(conn, *line, *c);
}
////////////////////////////////////////////////////////////////////////////////
Route::Response
LineRoute::impl(HttpDelete, const Request& req, int pid, int p, int lid) const
{
  LockedSession session(get_session(req));
  auto conn = must_get_connection();
  auto page = session->must_find(conn, pid, p);
  if (!page->book().is_book()) {
    THROW(BadRequest,
          "(LineRoute) cannot delete line from project (must be a book)");
  }
  MysqlCommiter commiter(conn);
  delete_line(conn.db(), pid, p, lid);
  page->delete_line(lid);
  commiter.commit();
  return ok();
}

////////////////////////////////////////////////////////////////////////////////
Route::Response
LineRoute::impl(HttpGet, const Request& req, int pid, int p, int lid, int tid)
  const
{
  LockedSession session(get_session(req));
  auto conn = must_get_connection();
  auto line = session->must_find(conn, pid, p, lid);
  auto token = find_token(*line, tid);
  if (not token) {
    THROW(NotFound, "(LineRoute) cannot find token id ", tid);
  }
  Json j;
  return j << *token;
}

////////////////////////////////////////////////////////////////////////////////
Route::Response
LineRoute::impl(HttpPost, const Request& req, int pid, int p, int lid, int tid)
  const
{
  const auto json = crow::json::load(req.body);
  const auto c = get<std::string>(json, "correction");
  if (not c)
    THROW(BadRequest, "(LineRoute) missing correction data");
  LockedSession session(get_session(req));
  auto conn = must_get_connection();
  auto line = session->must_find(conn, pid, p, lid);
  return correct(conn, *line, tid, *c);
}

////////////////////////////////////////////////////////////////////////////////
Route::Response
LineRoute::correct(MysqlConnection& conn,
                   Line& line,
                   const std::string& c) const
{
  WagnerFischer wf;
  wf.set_gt(c);
  wf.set_ocr(line);
  const auto lev = wf();
  CROW_LOG_DEBUG << "(LineRoute) correction: " << c;
  CROW_LOG_DEBUG << "(LineRoute) line.cor(): " << line.cor();
  CROW_LOG_DEBUG << "(LineRoute) line.ocr(): " << line.ocr();
  CROW_LOG_DEBUG << "(LineRoute)        lev: " << lev;
  wf.correct(line);
  update_line(conn, line);
  Json j;
  return j << line;
}

////////////////////////////////////////////////////////////////////////////////
Route::Response
LineRoute::correct(MysqlConnection& conn,
                   Line& line,
                   int tid,
                   const std::string& c) const
{
  auto token = find_token(line, tid);
  if (not token) {
    THROW(NotFound,
          "(LineRoute) cannot find ",
          line.page().book().id(),
          "-",
          line.page().id(),
          "-",
          line.id(),
          "-",
          tid);
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
    THROW(Error,
          "(LineRoute) cannot find token ",
          line.page().book().id(),
          "-",
          line.page().id(),
          "-",
          line.id(),
          "-",
          tid,
          " after correction");
  }
  update_line(conn, line);
  Json j;
  return j << *token;
}

////////////////////////////////////////////////////////////////////////////////
boost::optional<Token>
LineRoute::find_token(const Line& line, int tid)
{
  boost::optional<Token> token;
  line.each_token([&](const auto& t) {
    if (t.id == tid) {
      token = t;
    }
  });
  return token;
}

////////////////////////////////////////////////////////////////////////////////
void
LineRoute::update_line(MysqlConnection& conn, const Line& line)
{
  MysqlCommiter commiter(conn);
  pcw::update_line(conn.db(), line);
  commiter.commit();
}
