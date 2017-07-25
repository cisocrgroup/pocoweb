#include "LineRoute.hpp"
#include <crow.h>
#include <unicode/uchar.h>
#include <utf8.h>
#include <regex>
#include "core/Book.hpp"
#include "core/BookDirectoryBuilder.hpp"
#include "core/Package.hpp"
#include "core/Page.hpp"
#include "core/Session.hpp"
#include "core/SessionDirectory.hpp"
#include "core/User.hpp"
#include "core/WagnerFischer.hpp"
#include "core/jsonify.hpp"
#include "utils/Error.hpp"
#include "utils/ScopeGuard.hpp"

#define LINE_ROUTE_ROUTE "/books/<int>/pages/<int>/lines/<int>"

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
const char* LineRoute::route_ = LINE_ROUTE_ROUTE;
const char* LineRoute::name_ = "LineRoute";

////////////////////////////////////////////////////////////////////////////////
void LineRoute::Register(App& app) {
	CROW_ROUTE(app, LINE_ROUTE_ROUTE).methods("GET"_method)(*this);
}

////////////////////////////////////////////////////////////////////////////////
Route::Response LineRoute::impl(HttpGet, const Request& req, int bid, int pid,
				int lid) const {
	LockedSession session(must_find_session(req));
	auto conn = must_get_connection();
	auto line = session->must_find(conn, bid, pid, lid);
	Json j;
	return j << *line;
}
