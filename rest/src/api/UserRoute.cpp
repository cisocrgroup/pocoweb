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
	auto json = crow::json::load(req.body);
	const auto name = get<std::string>(json, "name");
	const auto email = get<std::string>(json, "email");
	const auto pass = get<std::string>(json, "pass");
	const auto inst = get<std::string>(json, "institute");
	const auto admin = get<bool>(json, "admin");
	if (not name or not email or not pass or not inst or not admin)
		THROW(BadRequest, "invalid data for new user");
	MysqlCommiter commiter(conn);
	const auto user =
	    pcw::create_user(conn.db(), *name, *pass, *email, *inst, *admin);
	assert(user);
	commiter.commit();
	Json j;
	return j << *user;
}

////////////////////////////////////////////////////////////////////////////////
Route::Response UserRoute::impl(HttpDelete, const Request& req, int uid) const {
	LockedSession session(must_find_session(req));
	auto conn = must_get_connection();
	auto user = select_user(conn.db(), uid);
	if (not user) THROW(NotFound, "invalid user id: ", uid);
	if (user->admin() and user->id() != session->user().id()) {
		THROW(Forbidden, "user ", session->user().name,
		      " cannot delete user ", user->name);
	}
	if (not session->user().admin() and
	    user->id() != session->user().id()) {
		THROW(Forbidden, "user ", session->user().name,
		      " cannot delete user ", user->name);
	}
	MysqlCommiter commiter(conn);
	tables::Projects p;
	using namespace sqlpp;
	auto ps =
	    conn.db()(select(p.projectid).from(p).where(p.owner == user->id()));
	for (const auto& pp : ps) {
		delete_project(conn.db(), pp.projectid);
		session->uncache_project(pp.projectid);
	}
	delete_user(conn.db(), user->id());
	commiter.commit();
	return ok();
}

////////////////////////////////////////////////////////////////////////////////
Route::Response UserRoute::impl(HttpGet, const Request& req, int uid) const {
	LockedSession session(must_find_session(req));
	auto conn = must_get_connection();
	auto user = select_user(conn.db(), uid);
	if (not user) {
		THROW(NotFound, "invalid user id: ", uid);
	}
	if (not session->user().admin()) {
		THROW(Forbidden, "only admins can get user credentials");
	}
	Json j;
	return j << *user;
}

////////////////////////////////////////////////////////////////////////////////
Route::Response UserRoute::impl(HttpPost, const Request& req, int uid) const {
	LockedSession session(must_find_session(req));
	auto conn = must_get_connection();
	auto user = select_user(conn.db(), uid);
	if (not user) {
		THROW(NotFound, "invalid user id: ", uid);
	}
	if (user->admin() and session->user().id() != user->id()) {
		THROW(Forbidden, "user ", session->user().name,
		      " cannot update user ", user->name);
	}
	if (not session->user().admin() and
	    session->user().id() != user->id()) {
		THROW(Forbidden, "user ", session->user().name,
		      " cannot update user ", user->name);
	}
	auto json = crow::json::load(req.body);
	const auto name = get<std::string>(json, "name");
	const auto email = get<std::string>(json, "email");
	const auto pass = get<std::string>(json, "pass");
	const auto inst = get<std::string>(json, "institute");
	// set values only if not empty
	if (email and *email != "") user->email = *email;
	if (name and *name != "") user->name = *name;
	if (inst and *inst != "") user->institute = *inst;
	if (pass and *pass != "") user->password = Password::make(*pass);
	MysqlCommiter commiter(conn);
	update_user(conn.db(), *user);
	if (session->user().id() == user->id()) {
		session->set_user(*user);
	}
	commiter.commit();
	Json j;
	return j << *user;
}
