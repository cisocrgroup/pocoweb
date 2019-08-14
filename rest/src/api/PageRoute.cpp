#include "PageRoute.hpp"
#include "core/Book.hpp"
#include "core/BookDirectoryBuilder.hpp"
#include "core/Package.hpp"
#include "core/Page.hpp"
#include "core/Session.hpp"
#include "core/WagnerFischer.hpp"
#include "core/jsonify.hpp"
#include "database/DbStructs.hpp"
#include "utils/Error.hpp"
#include "utils/ScopeGuard.hpp"
#include <crow.h>
#include <regex>

#define PAGE_ROUTE_ROUTE_1 "/books/<int>/pages/<int>"
#define PAGE_ROUTE_ROUTE_2 "/books/<int>/pages/first"
#define PAGE_ROUTE_ROUTE_3 "/books/<int>/pages/last"
#define PAGE_ROUTE_ROUTE_4 "/books/<int>/pages/<int>/next/<int>"
#define PAGE_ROUTE_ROUTE_5 "/books/<int>/pages/<int>/prev/<int>"

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
const char *PageRoute::route_ =
    PAGE_ROUTE_ROUTE_1 "," PAGE_ROUTE_ROUTE_2 "," PAGE_ROUTE_ROUTE_3
                       "," PAGE_ROUTE_ROUTE_4 "," PAGE_ROUTE_ROUTE_5;
const char *PageRoute::name_ = "PageRoute";

////////////////////////////////////////////////////////////////////////////////
void PageRoute::Register(App &app) {
  CROW_ROUTE(app, PAGE_ROUTE_ROUTE_1)
      .methods("GET"_method, "DELETE"_method)(*this);
  CROW_ROUTE(app, PAGE_ROUTE_ROUTE_2).methods("GET"_method)(*this);
  CROW_ROUTE(app, PAGE_ROUTE_ROUTE_3).methods("GET"_method)(*this);
  CROW_ROUTE(app, PAGE_ROUTE_ROUTE_4).methods("GET"_method)(*this);
  CROW_ROUTE(app, PAGE_ROUTE_ROUTE_5).methods("GET"_method)(*this);
}

////////////////////////////////////////////////////////////////////////////////
// GET /books/<bid>/pages/<pid>
////////////////////////////////////////////////////////////////////////////////
Route::Response PageRoute::impl(HttpGet, const Request &req, int bid,
                                int pid) const {
  CROW_LOG_INFO << "(PageRoute) GET page " << bid << ":" << pid;
  DbPage page(bid, pid);
  auto conn = must_get_connection();
  if (not page.load(conn)) {
    THROW(NotFound, "(PageRoute) cannot find page ", bid, ":", pid);
  }
  Json j;
  return j << page;
}

////////////////////////////////////////////////////////////////////////////////
// DELETE /books/<bid>/pages/<pid>
////////////////////////////////////////////////////////////////////////////////
Route::Response PageRoute::impl(HttpDelete, const Request &req, int bid,
                                int pid) const {
  LockedSession session(get_session(req));
  auto conn = must_get_connection();
  CROW_LOG_DEBUG << "(PageRoute) searching for book id: " << bid
                 << " page id: " << pid;
  const auto book = session->must_find(conn, bid);
  if (!book->is_book()) {
    THROW(BadRequest,
          "(PageRoute) cannot delete page from project (must be a book)");
  }
  const auto page = session->must_find(conn, bid, pid);
  MysqlCommitter committer(conn);
  delete_page(conn.db(), bid, pid);
  book->delete_page(page->id());
  committer.commit();
  return ok();
}

////////////////////////////////////////////////////////////////////////////////
// GET /books/<bid>/pages/{first,last}
////////////////////////////////////////////////////////////////////////////////
Route::Response PageRoute::impl(HttpGet, const Request &req, int bid) const {
  if (strcasestr(req.url.data(), "/first")) {
    return first(req, bid);
  } else if (strcasestr(req.url.data(), "/last")) {
    return last(req, bid);
  } else {
    return bad_request();
  }
}

////////////////////////////////////////////////////////////////////////////////
Route::Response PageRoute::last(const Request &req, int bid) const {
  auto conn = must_get_connection();
  using namespace sqlpp;
  tables::ProjectPages p;
  auto rows = conn.db()(select(p.pageid).from(p).where(
      p.projectid == bid and p.pageid == p.nextpageid));
  if (rows.empty()) {
    THROW(NotFound, "(PageRoute) cannot find last page for package: ", bid);
  }
  CROW_LOG_INFO << "(PageRoute) last page for package " << bid << ": "
                << rows.front().pageid;
  return impl(HttpGet{}, req, bid, rows.front().pageid);
}

////////////////////////////////////////////////////////////////////////////////
Route::Response PageRoute::first(const Request &req, int bid) const {
  auto conn = must_get_connection();
  using namespace sqlpp;
  tables::ProjectPages p;
  auto rows = conn.db()(select(p.pageid).from(p).where(
      p.projectid == bid and p.pageid == p.prevpageid));
  if (rows.empty()) {
    THROW(NotFound, "(PageRoute) cannot find first page for package: ", bid);
  }
  CROW_LOG_INFO << "(PageRoute) first page for package " << bid << ": "
                << rows.front().pageid;
  return impl(HttpGet{}, req, bid, rows.front().pageid);
}

////////////////////////////////////////////////////////////////////////////////
// GET /books/<bid>/pages/<pid>/{next,prev}/<n>
////////////////////////////////////////////////////////////////////////////////
Route::Response PageRoute::impl(HttpGet, const Request &req, int bid, int pid,
                                int n) const {
  // LockedSession session(get_session(req));
  // auto conn = must_get_connection();
  // auto book = session->must_find(conn, bid);
  if (strcasestr(req.url.data(), "/next/")) {
    return next(req, bid, pid, std::abs(n));
  } else if (strcasestr(req.url.data(), "/prev/")) {
    return prev(req, bid, pid, std::abs(n));
  }
  return bad_request();
}

////////////////////////////////////////////////////////////////////////////////
Route::Response PageRoute::next(const Request &req, int bid, int pid,
                                int n) const {
  auto conn = must_get_connection();
  using namespace sqlpp;
  tables::ProjectPages p;
  auto rows = conn.db()(select(p.pageid)
                            .from(p)
                            .where(p.projectid == bid and p.pageid > pid)
                            .order_by(p.pageid.asc()));
  auto npid = pid;
  for (const auto &row : rows) {
    if (n-- <= 0) {
      break;
    }
    npid = row.pageid;
  }
  return impl(HttpGet{}, req, bid, npid);
}

////////////////////////////////////////////////////////////////////////////////
Route::Response PageRoute::prev(const Request &req, int bid, int pid,
                                int n) const {
  auto conn = must_get_connection();
  using namespace sqlpp;
  tables::ProjectPages p;
  auto rows = conn.db()(select(p.pageid)
                            .from(p)
                            .where(p.projectid == bid and p.pageid < pid)
                            .order_by(p.pageid.desc()));
  auto ppid = pid;
  for (const auto &row : rows) {
    if (n-- <= 0) {
      break;
    }
    ppid = row.pageid;
  }
  return impl(HttpGet{}, req, bid, ppid);
}

////////////////////////////////////////////////////////////////////////////////
Route::Response PageRoute::print(Json &json, int pid, const Page &page,
                                 const Project &project) {
  wj(json, page, pid);
  const auto nextpage = project.next(page.id(), 1);
  const auto prevpage = project.next(page.id(), -1);
  const auto nextpageid = nextpage ? nextpage->id() : 0;
  const auto prevpageid = prevpage ? prevpage->id() : 0;
  json["nextPageId"] = nextpageid;
  json["prevPageId"] = prevpageid;
  json["projectId"] = pid;
  json["bookId"] = project.origin().id();
  return json;
}
