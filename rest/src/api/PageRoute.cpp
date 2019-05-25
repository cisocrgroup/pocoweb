#include "PageRoute.hpp"
#include "core/Book.hpp"
#include "core/BookDirectoryBuilder.hpp"
#include "core/Package.hpp"
#include "core/Page.hpp"
#include "core/Session.hpp"
#include "core/WagnerFischer.hpp"
#include "core/jsonify.hpp"
#include "utils/Error.hpp"
#include "utils/ScopeGuard.hpp"
#include <crow.h>
#include <regex>

#define PAGE_ROUTE_ROUTE_1 "/books/<int>/pages/<int>"
#define PAGE_ROUTE_ROUTE_2 "/books/<int>/pages/<int>/lines"
#define PAGE_ROUTE_ROUTE_3 "/books/<int>/pages/first"
#define PAGE_ROUTE_ROUTE_4 "/books/<int>/pages/last"
#define PAGE_ROUTE_ROUTE_5 "/books/<int>/pages/<int>/next/<int>"
#define PAGE_ROUTE_ROUTE_6 "/books/<int>/pages/<int>/prev/<int>"

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
const char* PageRoute::route_ = PAGE_ROUTE_ROUTE_1
  "," PAGE_ROUTE_ROUTE_2 "," PAGE_ROUTE_ROUTE_3 "," PAGE_ROUTE_ROUTE_4
  "," PAGE_ROUTE_ROUTE_5 "," PAGE_ROUTE_ROUTE_6;
const char* PageRoute::name_ = "PageRoute";

////////////////////////////////////////////////////////////////////////////////
void
PageRoute::Register(App& app)
{
  CROW_ROUTE(app, PAGE_ROUTE_ROUTE_1)
    .methods("GET"_method, "DELETE"_method)(*this);
  CROW_ROUTE(app, PAGE_ROUTE_ROUTE_2).methods("GET"_method)(*this);
  CROW_ROUTE(app, PAGE_ROUTE_ROUTE_3).methods("GET"_method)(*this);
  CROW_ROUTE(app, PAGE_ROUTE_ROUTE_4).methods("GET"_method)(*this);
  CROW_ROUTE(app, PAGE_ROUTE_ROUTE_5).methods("GET"_method)(*this);
  CROW_ROUTE(app, PAGE_ROUTE_ROUTE_6).methods("GET"_method)(*this);
}

////////////////////////////////////////////////////////////////////////////////
Route::Response
PageRoute::impl(HttpGet, const Request& req, int bid, int pid) const
{
  LockedSession session(get_session(req));
  auto conn = must_get_connection();
  CROW_LOG_DEBUG << "(PageRoute) searching for book id: " << bid
                 << " page id: " << pid;
  const auto page = session->must_find(conn, bid, pid);
  Json j;
  return print(j, bid, *page, page->book());
}

////////////////////////////////////////////////////////////////////////////////
Route::Response
PageRoute::impl(HttpDelete, const Request& req, int bid, int pid) const
{
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
  MysqlCommiter commiter(conn);
  delete_page(conn.db(), bid, pid);
  book->delete_page(page->id());
  commiter.commit();
  return ok();
}

////////////////////////////////////////////////////////////////////////////////
Route::Response
PageRoute::impl(HttpGet, const Request& req, int bid) const
{
  LockedSession session(get_session(req));
  auto conn = must_get_connection();
  auto book = session->must_find(conn, bid);
  CROW_LOG_DEBUG << "(PageRoute) found project id: " << book->id();
  bool first = false;
  if (strcasestr(req.url.data(), "/first"))
    first = true;
  else if (strcasestr(req.url.data(), "/last"))
    first = false;
  else
    return bad_request();
  if (book->empty())
    return not_found();
  CROW_LOG_DEBUG << "(PageRoute) project id: " << book->id()
                 << " size: " << book->size();
  for (const auto p : *book) {
    CROW_LOG_DEBUG << "(PageRoute) page id: " << p->id();
  }

  Json j;
  if (first) {
    CROW_LOG_DEBUG << "(PageRoute) project id: " << book->id()
                   << " pageid: " << book->front()->id()
                   << " lines: " << book->front()->size();
    return print(j, bid, *book->front(), *book);
  } else {
    CROW_LOG_DEBUG << "(PageRoute) project id: " << book->id()
                   << " pageid: " << book->back()->id()
                   << " lines: " << book->back()->size();
    return print(j, bid, *book->back(), *book);
  }
}

////////////////////////////////////////////////////////////////////////////////
Route::Response
PageRoute::impl(HttpGet, const Request& req, int bid, int pid, int val) const
{
  LockedSession session(get_session(req));
  auto conn = must_get_connection();
  auto book = session->must_find(conn, bid);
  if (strcasestr(req.url.data(), "/next/"))
    return next(*book, pid, std::abs(val));
  else if (strcasestr(req.url.data(), "/prev/"))
    return prev(*book, pid, std::abs(val));
  else
    return bad_request();
}

////////////////////////////////////////////////////////////////////////////////
Route::Response
PageRoute::next(const Project& book, int pid, int val) const
{
  auto page = book.next(pid, val);
  if (not page)
    return not_found();
  Json j;
  return print(j, pid, *page, book);
}

////////////////////////////////////////////////////////////////////////////////
Route::Response
PageRoute::prev(const Project& book, int pid, int val) const
{
  auto page = book.next(pid, -val);
  if (not page)
    return not_found();
  Json j;
  return print(j, pid, *page, book);
}

////////////////////////////////////////////////////////////////////////////////
Route::Response
PageRoute::print(Json& json, int pid, const Page& page, const Project& project)
{
  json << page;
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
