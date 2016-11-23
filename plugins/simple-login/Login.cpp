#include <cppconn/connection.h>
#include <crow.h>
#include "core/Database.hpp"
#include "core/Sessions.hpp"
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
crow::response
Login::operator()(const std::string& name, const std::string& pass) const
{
	auto session = sessions().new_session();
	if (not session)
		return internal_server_error();
	auto db = database(session);
	std::lock_guard<std::mutex> lock(db.session().mutex);
	auto user = db.authenticate(name, pass);
	if (not user)
		return forbidden();
	session->user = user;
	auto response = ok();
	set_session_id(response, session->sid);
	return response;
}
