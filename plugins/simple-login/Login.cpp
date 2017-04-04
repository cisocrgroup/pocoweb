#include <crow.h>
#include "database/NewDatabase.hpp"
#include "core/Session.hpp"
#include "Login.hpp"

using namespace pcw;

#define LOGIN_ROUTE1 "/login/user/<string>"
#define LOGIN_ROUTE2 "/login/user/<string>/pass/<string>"

////////////////////////////////////////////////////////////////////////////////
const char* Login::route_ = LOGIN_ROUTE1 "," LOGIN_ROUTE2;

////////////////////////////////////////////////////////////////////////////////
const char* Login::name_ = "Login";

////////////////////////////////////////////////////////////////////////////////
void
Login::Register(App& app)
{
	CROW_ROUTE(app, LOGIN_ROUTE1)(*this);
	CROW_ROUTE(app, LOGIN_ROUTE2)(*this);
}

////////////////////////////////////////////////////////////////////////////////
Route::Response
Login::impl(HttpGet, const Request& req,
		const std::string& name, const std::string& pass) const
{
	auto conn = connection();
	assert(conn);

	auto user = select_user(conn.db(), name);
	if (not user or not user->password.authenticate(pass)) {
		CROW_LOG_ERROR << "invalid user: " << name;
		return forbidden();
	}

	auto session = new_session(*user);
	if (not session) {
		CROW_LOG_ERROR << "could not create new session";
		return internal_server_error();
	}
	SessionLock lock(*session);
	using namespace std::literals::chrono_literals;
	session->set_expiration_date_from_now(24h);
	auto response = ok();
	set_session_id(response, session->id());
	return response;
}

////////////////////////////////////////////////////////////////////////////////
Route::Response
Login::impl(HttpGet, const Request& req, const std::string& name) const
{
	try {
		auto session = this->session(req);
		if (session) {
			SessionLock lock(*session);
			return session->user().name == name ? ok() : forbidden();
		}
	} catch (const Forbidden& f) {
		CROW_LOG_ERROR << f.what();
	}
	return forbidden();
}
