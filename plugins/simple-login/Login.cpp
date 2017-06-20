#include <crow.h>
#include "database/Database.hpp"
#include "core/Session.hpp"
#include "Login.hpp"

using namespace pcw;

#define LOGIN_ROUTE "/login"

////////////////////////////////////////////////////////////////////////////////
const char* Login::route_ = LOGIN_ROUTE;

////////////////////////////////////////////////////////////////////////////////
const char* Login::name_ = "Login";

////////////////////////////////////////////////////////////////////////////////
void
Login::Register(App& app)
{
	CROW_ROUTE(app, LOGIN_ROUTE)(*this);
}

////////////////////////////////////////////////////////////////////////////////
Route::Response
Login::login(const Request& req, const std::string& name, const std::string& pass) const
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
Login::impl(HttpGet get, const Request& req) const
{
	auto name = req.get_header_value("name");
	auto pass = req.get_header_value("pass");
	if (not name.empty() and not pass.empty()) {
		return login(req, name, pass);
	} else {
		CROW_LOG_ERROR << "invalid login data for user '"
			       << name << "'";
		return bad_request();
	}
}
