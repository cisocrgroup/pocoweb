#include <boost/algorithm/string/predicate.hpp>
#include <crow.h>
#include <cppconn/connection.h>
#include "Database.hpp"
#include "Sessions.hpp"
#include "User.hpp"
#include "UpdateUser.hpp"

using namespace pcw;

#define UPDATE_USER_ROUTE "/update-user/<string>/<string>"

////////////////////////////////////////////////////////////////////////////////
const char* UpdateUser::route_ = UPDATE_USER_ROUTE;

////////////////////////////////////////////////////////////////////////////////
const char* UpdateUser::name_ = "UpdateUser";

////////////////////////////////////////////////////////////////////////////////
void 
UpdateUser::Register(App& app) 
{
	CROW_ROUTE(app, UPDATE_USER_ROUTE)(*this);
}

////////////////////////////////////////////////////////////////////////////////
crow::response 
UpdateUser::operator()(
	const crow::request& request, 
	const std::string& what, 
	const std::string& val
) const {
	static const std::string institute = "institute";
	static const std::string email = "email";

	// get session and user
	auto session = this->session(request);
	if (not session or not session->user)
		return forbidden();

	// check which field to set 
	if (boost::iequals(what, institute)) 
		session->user->institute = val;
	else if (boost::iequals(what, email))
		session->user->email = val;
	else
		return bad_request();		
	
	// update user information
	auto db = database(request);
	if (not db) // this should not be possible
		return internal_server_error();

	db.get().update(*session->user);
	return ok();
}
