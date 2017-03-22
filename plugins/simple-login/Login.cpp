#include <cppconn/connection.h>
#include <crow.h>
#include "database/NewDatabase.hpp"
#include "core/Session.hpp"
#include "Login.hpp"

using namespace pcw;

#define LOGIN_ROUTE "/login/user/<string>/pass/<string>"

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
Login::impl(
	HttpGet,
	const Request& req,
	const std::string& name,
	const std::string& pass
) const
{
	auto conn = connection();
	assert(conn);

	auto user = login_user(conn.db(), name, pass);
	if (not user)
		THROW(Forbidden, "user: ", name, ": could not be authenticated");
	auto session = new_session(*user);
	if (not session)
		THROW(Error, "could not create new session");

	SessionLock lock(*session);
	using namespace std::literals::chrono_literals;
	session->set_expiration_date_from_now(24h);
	auto response = ok();
	set_session_id(response, session->id());
	return response;
}
