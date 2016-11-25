#include <cppconn/connection.h>
#include <regex>
#include <crow.h>
#include "core/jsonify.hpp"
#include "core/Error.hpp"
#include "core/User.hpp"
#include "core/Page.hpp"
#include "core/Book.hpp"
#include "core/Database.hpp"
#include "PageRoute.hpp"
#include "core/Sessions.hpp"
#include "core/Package.hpp"
#include "core/ScopeGuard.hpp"
#include "core/BookDirectoryBuilder.hpp"
#include "core/WagnerFischer.hpp"

#define PAGE_ROUTE_ROUTE_1 "/books/<int>/pages/<int>"
#define PAGE_ROUTE_ROUTE_2 "/books/<int>/pages/<int>/lines"
#define PAGE_ROUTE_ROUTE_3 "/books/<int>/pages/<string>"
#define PAGE_ROUTE_ROUTE_4 "/books/<int>/pages/<int>/<string>/<int>"

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
const char* PageRoute::route_ =
	PAGE_ROUTE_ROUTE_1 ","
	PAGE_ROUTE_ROUTE_2 ","
	PAGE_ROUTE_ROUTE_3 ","
	PAGE_ROUTE_ROUTE_4;
const char* PageRoute::name_ = "PageRoute";

////////////////////////////////////////////////////////////////////////////////
void
PageRoute::Register(App& app)
{
	CROW_ROUTE(app, PAGE_ROUTE_ROUTE_1).methods("GET"_method)(*this);
	CROW_ROUTE(app, PAGE_ROUTE_ROUTE_2).methods("GET"_method)(*this);
	CROW_ROUTE(app, PAGE_ROUTE_ROUTE_3).methods("GET"_method)(*this);
	CROW_ROUTE(app, PAGE_ROUTE_ROUTE_4).methods("GET"_method)(*this);
}

////////////////////////////////////////////////////////////////////////////////
Route::Response
PageRoute::impl(HttpGet, const Request& req, int bid, int pid) const
{
	auto db = database(req);
	std::lock_guard<std::mutex> lock(db.session().mutex);
	auto page = find(db, bid, pid);
	if (not page)
		return not_found();
	assert(page);
	// TODO missing authentication
	Json j;
	return j << *page;
}

////////////////////////////////////////////////////////////////////////////////
Route::Response
PageRoute::impl(HttpGet, const Request& req, int bid, const std::string& dir) const
{
	auto db = database(req);
	std::lock_guard<std::mutex> lock(db.session().mutex);
	auto book = find(db, bid);
	bool first = false;
	if (not book)
		return not_found();
	if (strcasecmp(dir.data(), "first") == 0)
		first = true;
	else if (strcasecmp(dir.data(), "last") == 0)
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
PageRoute::impl(HttpGet, const Request& req, int bid, int pid,
	const std::string& dir, int val
) const
{
	auto db = database(req);
	std::lock_guard<std::mutex> lock(db.session().mutex);
	auto book = find(db, bid);
	if (not book)
		return not_found();
	if (strcasecmp(dir.data(), "next") == 0)
		return next(*book, pid, std::abs(val));
	else if (strcasecmp(dir.data(), "prev") == 0)
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
