#include "core/User.hpp"
#include <crow.h>
#include <regex>
#include "UserRoute.hpp"
#include "core/Session.hpp"
#include "core/WagnerFischer.hpp"
#include "core/jsonify.hpp"
#include "utils/Error.hpp"
#include "utils/ScopeGuard.hpp"

#define USER_ROUTE_ROUTE_1 "/users"
#define USER_ROUTE_ROUTE_2 "/users/<int>"

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
const char* UserRoute::route_ = USER_ROUTE_ROUTE_1 "," USER_ROUTE_ROUTE_2;
const char* UserRoute::name_ = "UserRoute";

////////////////////////////////////////////////////////////////////////////////
void UserRoute::Register(App& app) {
	CROW_ROUTE(app, USER_ROUTE_ROUTE_1)
	    .methods("GET"_method, "POST"_method)(*this);
	CROW_ROUTE(app, USER_ROUTE_ROUTE_2)
	    .methods("GET"_method, "POST"_method, "DELETE"_method)(*this);
}

////////////////////////////////////////////////////////////////////////////////
Route::Response UserRoute::impl(HttpGet, const Request& req) const {
	LockedSession session(must_find_session(req));
	auto conn = must_get_connection();
	if (not session->user().admin()) {
		THROW(Forbidden, "only admins can list users");
	}
	auto users = select_all_users(conn.db());
	CROW_LOG_DEBUG << "(UserRoute) Loaded " << users.size() << " users";
	Json j;
	size_t i = 0;
	for (const auto& user : users) {
		assert(user);
		j["users"][i] << *user;
		i++;
	}
	return j;
}

////////////////////////////////////////////////////////////////////////////////
Route::Response UserRoute::impl(HttpPost, const Request& req) const {
	LockedSession session(must_find_session(req));
	auto conn = must_get_connection();
	if (not session->user().admin())
		THROW(Forbidden, "only admins can create new users");
	CROW_LOG_DEBUG << "(UserRoute) body: " << req.body;
	auto json = crow::json::load(req.body);
	const std::string name = json["name"].s();
	const std::string pass = json["pass"].s();
	const std::string email = json["email"].s();
	const std::string institute = json["institute"].s();
	const bool admin = json["admin"].b();
	if (name.empty() or pass.empty())
		THROW(BadRequest, "missing name and/or password for new user");
	MysqlCommiter commiter(conn);
	const auto user =
	    pcw::create_user(conn.db(), name, pass, email, institute, admin);
	assert(user);
	commiter.commit();
	Json j;
	return j << *user;
}

////////////////////////////////////////////////////////////////////////////////
Route::Response UserRoute::impl(HttpDelete, const Request& req, int uid) const {
	LockedSession session(must_find_session(req));
	auto conn = must_get_connection();
	if (not session->user().admin())
		THROW(Forbidden, "only admins can create new users");
	auto user = select_user(conn.db(), uid);
	if (not user) THROW(NotFound, "invalid user id: ", uid);
	if (user->admin()) THROW(Forbidden, "cannot delete admin account");
	MysqlCommiter commiter(conn);
	delete_user(conn.db(), uid);
	commiter.commit();
	return ok();
}

////////////////////////////////////////////////////////////////////////////////
[[noreturn]] Route::Response UserRoute::impl(HttpGet, const Request& req,
					     int uid) const {
	THROW(NotImplemented, "Not implemented: [GET] /users/uid");
}

////////////////////////////////////////////////////////////////////////////////
[[noreturn]] Route::Response UserRoute::impl(HttpPost, const Request& req,
					     int uid) const {
	THROW(NotImplemented, "Not implemented: [POST] /users/uid");
}
