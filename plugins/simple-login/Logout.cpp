#include <crow.h>
#include "Logout.hpp"
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
void
Logout::Register(App& app)
{
	CROW_ROUTE(app, LOGIN_ROUTE).methods("GET"_method)(*this);
}

////////////////////////////////////////////////////////////////////////////////
Route::Response
Logout::login(const Request& req, const std::string& name, const std::string& pass) const
{
	CROW_LOG_INFO << "(Logout) login attempt for user: " << name;

	// check for existing session
	auto session = find_session(req);
	if (session) {
		CROW_LOG_INFO << "(Logout) existing session sid: " << session->id();
		return ok();
	}

	// create new session
	auto conn = connection();
	assert(conn);
	auto user = select_user(conn.db(), name);
	if (not user) {
		CROW_LOG_ERROR << "(Logout) invalid user: " << name;
		return forbidden();
	}
	if (not user->password.authenticate(pass)) {
		CROW_LOG_ERROR << "(Logout) invalid password for user: " << name;
	}

	session = new_session(*user);
	if (not session) {
		CROW_LOG_ERROR << "(Logout) could not create new session";
		return internal_server_error();
	}
	SessionLock lock(*session);
	using namespace std::literals::chrono_literals;
	session->set_expiration_date_from_now(24h);
	auto response = ok();
	session->set_cookies(response);
	CROW_LOG_INFO << "(Logout) login successfull for user: " << name
		      << ", sid: " << session->id();
	return response;
}

////////////////////////////////////////////////////////////////////////////////
Route::Response
Logout::impl(HttpGet, const Request& req) const
{
	auto session = find_session(req);
	if (not session) {
		CROW_LOG_ERROR << "(Logout) not logged in";
		return bad_request();
	}
	SessionLock lock(*session);
	delete_session(*session);
	return ok();
}

