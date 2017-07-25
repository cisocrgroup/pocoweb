#include "Logout.hpp"
#include <crow.h>
#include "core/Session.hpp"
#include "core/jsonify.hpp"
#include "database/Database.hpp"

using namespace pcw;

#define LOGIN_ROUTE "/logout"

////////////////////////////////////////////////////////////////////////////////
const char* Logout::route_ = LOGIN_ROUTE;

////////////////////////////////////////////////////////////////////////////////
const char* Logout::name_ = "Logout";

////////////////////////////////////////////////////////////////////////////////
void Logout::Register(App& app) {
	CROW_ROUTE(app, LOGIN_ROUTE).methods("GET"_method)(*this);
}

////////////////////////////////////////////////////////////////////////////////
Route::Response Logout::impl(HttpGet, const Request& req) const {
	LockedSession session(must_find_session(req));
	delete_session(*session);
	return ok();
}
