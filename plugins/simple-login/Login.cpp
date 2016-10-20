#include <cppconn/connection.h>
#include <crow.h>
#include "Database.hpp"
#include "Sessions.hpp"
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
	if (not db)
		return internal_server_error();
	std::lock_guard<std::mutex> lock(db->session().mutex);
	auto user = db.get().authenticate(name, pass);	
	if (not user)
		return forbidden();
	session->user = user;
	auto response = ok();
	set_session_id(response, session->sid);
	return response;
}
