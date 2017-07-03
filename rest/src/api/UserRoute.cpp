#include <regex>
#include <crow.h>
#include "utils/Error.hpp"
#include "utils/ScopeGuard.hpp"
#include "utils/QueryParser.hpp"
#include "core/jsonify.hpp"
#include "core/User.hpp"
#include "core/WagnerFischer.hpp"
#include "core/Session.hpp"
#include "UserRoute.hpp"

#define USER_ROUTE_ROUTE_1 "/users"
#define USER_ROUTE_ROUTE_2 "/users/<int>"

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
const char* UserRoute::route_ =
	USER_ROUTE_ROUTE_1 ","
	USER_ROUTE_ROUTE_2;
const char* UserRoute::name_ = "UserRoute";

////////////////////////////////////////////////////////////////////////////////
void
UserRoute::Register(App& app)
{
	CROW_ROUTE(app, USER_ROUTE_ROUTE_1).methods("GET"_method, "POST"_method)(*this);
	CROW_ROUTE(app, USER_ROUTE_ROUTE_2).methods("GET"_method, "POST"_method)(*this);
}

////////////////////////////////////////////////////////////////////////////////
Route::Response
UserRoute::impl(HttpGet, const Request& req) const
{
	auto conn = connection();
	assert(conn);
	auto session = this->session(req);
	if (not session)
		THROW(BadRequest, "could not find session");
	assert(session);
	SessionLock lock(*session);
	if (not session->user().admin()) {
		THROW(Forbidden, "only admins can list users");
	}
	auto users = select_all_users(conn.db());
	CROW_LOG_DEBUG << "(UserRoute) Loaded " << users.size() << " users";
	Json j;
	size_t i = 0;
	for (const auto& user: users) {
		assert(user);
		j["users"][i] << *user;
		i++;
	}
	return j;
}

////////////////////////////////////////////////////////////////////////////////
[[noreturn]] Route::Response
UserRoute::impl(HttpPost, const Request& req) const
{
	THROW(NotImplemented, "Not implemented: UserRoute::impl");
}

////////////////////////////////////////////////////////////////////////////////
[[noreturn]] Route::Response
UserRoute::impl(HttpGet, const Request& req, int uid) const
{
	THROW(NotImplemented, "Not implemented: UserRoute::impl");
}
////////////////////////////////////////////////////////////////////////////////
[[noreturn]] Route::Response
UserRoute::impl(HttpPost, const Request& req, int uid) const
{
	THROW(NotImplemented, "Not implemented: UserRoute::impl");
}
