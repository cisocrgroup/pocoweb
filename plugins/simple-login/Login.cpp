#include "Login.hpp"
#include <crow.h>
#include "core/Session.hpp"
#include "core/jsonify.hpp"
#include "database/Database.hpp"

using namespace pcw;

#define LOGIN_ROUTE "/login"

////////////////////////////////////////////////////////////////////////////////
const char* Login::route_ = LOGIN_ROUTE;

////////////////////////////////////////////////////////////////////////////////
const char* Login::name_ = "Login";

////////////////////////////////////////////////////////////////////////////////
void Login::Register(App& app) {
	CROW_ROUTE(app, LOGIN_ROUTE)
	    .methods("POST"_method, "GET"_method)(*this);
}

////////////////////////////////////////////////////////////////////////////////
Route::Response Login::login(const Request& req, const std::string& name,
			     const std::string& pass) const {
	CROW_LOG_INFO << "(Login) login attempt for user: " << name;

	// check for existing session
	auto session = find_session(req);
	if (session) {
		LockedSession s(session);
		CROW_LOG_INFO << "(Login) existing session sid: " << s->id();
		return ok();
	}

	// create new session
	auto conn = must_get_connection();
	auto user = select_user(conn.db(), name);
	if (not user) {
		CROW_LOG_ERROR << "(Login) invalid user: " << name;
		return forbidden();
	}
	if (not user->password.authenticate(pass)) {
		CROW_LOG_ERROR << "(Login) invalid password for user: " << name;
		return forbidden();
	}
	session = new_session(*user);
	if (not session) {
		CROW_LOG_ERROR << "(Login) could not create new session";
		return internal_server_error();
	}
	LockedSession s(session);
	Json json;
	json["sid"] = s->id();
	return json;
}

////////////////////////////////////////////////////////////////////////////////
Route::Response Login::impl(HttpPost, const Request& req) const {
	CROW_LOG_DEBUG << "(Login) body: " << req.body;
	auto json = crow::json::load(req.body);
	if (not json) THROW(BadRequest, "bad json request data");
	if (json["name"].s().size() and json["pass"].s().size()) {
		return login(req, json["name"].s(), json["pass"].s());
	} else {
		THROW(BadRequest, "invalid login attempt");
	}
}

////////////////////////////////////////////////////////////////////////////////
Route::Response Login::impl(HttpGet, const Request& req) const {
	LockedSession session(must_find_session(req));
	Json j;
	return j << session->user();
}
