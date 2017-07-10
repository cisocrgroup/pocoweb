#include <regex>
#include <crow.h>
#include "utils/Error.hpp"
#include "utils/ScopeGuard.hpp"
#include "core/jsonify.hpp"
#include "core/User.hpp"
#include "core/Page.hpp"
#include "core/Book.hpp"
#include "PageRoute.hpp"
#include "core/Session.hpp"
#include "core/Package.hpp"
#include "core/BookDirectoryBuilder.hpp"
#include "core/WagnerFischer.hpp"

#define PAGE_ROUTE_ROUTE_1 "/books/<int>/pages/<int>"
#define PAGE_ROUTE_ROUTE_2 "/books/<int>/pages/<int>/lines"
#define PAGE_ROUTE_ROUTE_3 "/books/<int>/pages/first"
#define PAGE_ROUTE_ROUTE_4 "/books/<int>/pages/last"
#define PAGE_ROUTE_ROUTE_5 "/books/<int>/pages/<int>/next/<int>"
#define PAGE_ROUTE_ROUTE_6 "/books/<int>/pages/<int>/prev/<int>"

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
const char* PageRoute::route_ =
	PAGE_ROUTE_ROUTE_1 ","
	PAGE_ROUTE_ROUTE_2 ","
	PAGE_ROUTE_ROUTE_3 ","
	PAGE_ROUTE_ROUTE_4 ","
	PAGE_ROUTE_ROUTE_5 ","
	PAGE_ROUTE_ROUTE_6;
const char* PageRoute::name_ = "PageRoute";

////////////////////////////////////////////////////////////////////////////////
void
PageRoute::Register(App& app)
{
	CROW_ROUTE(app, PAGE_ROUTE_ROUTE_1).methods("GET"_method)(*this);
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
	auto conn = connection();
	auto session = this->session(req);
	assert(conn);
	assert(session);
	SessionLock lock(*session);
	CROW_LOG_DEBUG << "(PageRoute) searching for book id: " << bid << " page id: " << pid;
	auto project = session->find(conn, bid);
	auto page = session->find(conn, bid, pid);
	if (not page or not project)
		return not_found();
	assert(page);
	assert(project);
	Json j;
	return print(j, *page, *project);
}

////////////////////////////////////////////////////////////////////////////////
Route::Response
PageRoute::impl(HttpGet, const Request& req, int bid) const
{
	auto conn = connection();
	auto session = this->session(req);
	assert(conn);
	assert(session);
	SessionLock lock(*session);

	auto book = session->find(conn, bid);
	CROW_LOG_DEBUG << "(PageRoute) found book id: " << book->id();
	bool first = false;
	if (not book)
		return not_found();
	if (strcasestr(req.url.data(), "/first"))
		first = true;
	else if (strcasestr(req.url.data(), "/last"))
		first = false;
	else
		return bad_request();
	if (book->empty())
		return not_found();
	CROW_LOG_DEBUG << "(PageRoute) book id: " << book->id()
		       << " size: " << book->size();
	for (const auto p: *book) {
		CROW_LOG_DEBUG << "(PageRoute) page id: " << p->id();
	}

	Json j;
	if (first) {
		CROW_LOG_DEBUG << "(PageRoute) bookid: " << book->id()
			       << " pageid: " << book->front()->id()
			       << " lines: " << book->front()->size();
		return print(j, *book->front(), *book);
	} else {
		CROW_LOG_DEBUG << "(PageRoute) bookid: " << book->id()
			       << " pageid: " << book->back()->id()
			       << " lines: " << book->back()->size();
		return print(j, *book->back(), *book);
	}
}

////////////////////////////////////////////////////////////////////////////////
Route::Response
PageRoute::impl(HttpGet, const Request& req, int bid, int pid, int val) const
{
	auto conn = connection();
	auto session = this->session(req);
	assert(conn);
	assert(session);
	SessionLock lock(*session);

	auto book = session->find(conn, bid);
	if (not book)
		return not_found();
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
	return print(j, *page, book);
}

////////////////////////////////////////////////////////////////////////////////
Route::Response
PageRoute::prev(const Project& book, int pid, int val) const
{
	auto page = book.next(pid, -val);
	if (not page)
		return not_found();
	Json j;
	return print(j, *page, book);
}

////////////////////////////////////////////////////////////////////////////////
Route::Response
PageRoute::print(Json& json, const Page& page, const Project& project)
{
	json << page;
	const auto nextpage = project.next(page.id(), 1);
	const auto prevpage = project.next(page.id(), -1);
	const auto nextpageid = nextpage ? nextpage->id() : 0;
	const auto prevpageid = prevpage ? prevpage->id() : 0;
	json["nextPageId"] = nextpageid;
	json["prevPageId"] = prevpageid;
	json["projectId"] = project.id();
	json["bookId"] = project.origin().id();
	return json;
}

