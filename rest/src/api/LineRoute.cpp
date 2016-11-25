#include <cppconn/connection.h>
#include <regex>
#include <crow.h>
#include "core/jsonify.hpp"
#include "core/Error.hpp"
#include "core/User.hpp"
#include "core/Page.hpp"
#include "core/Book.hpp"
#include "core/Database.hpp"
#include "LineRoute.hpp"
#include "core/Sessions.hpp"
#include "core/Package.hpp"
#include "core/ScopeGuard.hpp"
#include "core/BookDirectoryBuilder.hpp"
#include "core/WagnerFischer.hpp"

#define LINE_ROUTE_ROUTE "books/<int>/pages/<int>/lines/<int>"

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
const char* LineRoute::route_ = LINE_ROUTE_ROUTE;
const char* LineRoute::name_ = "LineRoute";

////////////////////////////////////////////////////////////////////////////////
void
LineRoute::Register(App& app)
{
	CROW_ROUTE(app, LINE_ROUTE_ROUTE).methods("GET"_method, "PUT"_method)(*this);
}

////////////////////////////////////////////////////////////////////////////////
Route::Response
LineRoute::impl(HttpGet, const Request& req, int bid, int pid, int lid) const
{
	auto db = database(req);
	std::lock_guard<std::mutex> lock(db.session().mutex);
	auto line = find(db, bid, pid, lid);
	assert(line);
	Json j;
	return j << *line;
}

////////////////////////////////////////////////////////////////////////////////
Route::Response
LineRoute::impl(HttpPut, const Request& req, int bid, int pid, int lid) const
{
	auto correction = req.url_params.get("correction");
	if (not correction)
		return bad_request();
	auto db = database(req);
	std::lock_guard<std::mutex> lock(db.session().mutex);
	auto line = find(db, bid, pid, lid);
	assert(line);

	CROW_LOG_DEBUG << "(LineRoute) correction: " << req.url_params.get("correction");
	WagnerFischer wf;
	wf.set_gt(correction);
	wf.set_ocr(*line);
	auto lev = wf();
	CROW_LOG_DEBUG << "(LineRoute) lev: " << lev << "\n" << wf;
	CROW_LOG_DEBUG << "(LineRoute) line: " << line->cor();
	wf.correct(*line);
	CROW_LOG_DEBUG << "(LineRoute) line: " << line->cor();
	db.set_autocommit(false);
	db.update_line(*line);
	db.commit();
	return ok();
}
