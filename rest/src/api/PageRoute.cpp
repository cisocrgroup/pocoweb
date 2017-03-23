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
	auto page = session->find(conn, bid, pid);
	if (not page)
		return not_found();
	assert(page);
	Json j;
	return j << *page;
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
	Json j;
	if (first)
		return j << *book->front();
	else
		return j << *book->back();
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
PageRoute::next(const BookView& book, int pid, int val) const
{
	auto page = book.next(pid, val);
	if (not page)
		return not_found();
	Json j;
	return j << *page;
}

////////////////////////////////////////////////////////////////////////////////
Route::Response
PageRoute::prev(const BookView& book, int pid, int val) const
{
	auto page = book.next(pid, -val);
	if (not page)
		return not_found();
	Json j;
	return j << *page;
}
