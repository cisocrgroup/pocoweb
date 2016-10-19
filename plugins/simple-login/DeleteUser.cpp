#include <crow.h>
#include <cppconn/connection.h>
#include "User.hpp"
#include "Sessions.hpp"
#include "Database.hpp"
#include "DeleteUser.hpp"

using namespace pcw;

#define DELETE_USER_ROUTE "/delete-user/<string>"

////////////////////////////////////////////////////////////////////////////////
const char* DeleteUser::route_ = DELETE_USER_ROUTE;

////////////////////////////////////////////////////////////////////////////////
const char* DeleteUser::name_ = "DeleteUser";

////////////////////////////////////////////////////////////////////////////////
void 
DeleteUser::Register(App& app) 
{
	CROW_ROUTE(app, DELETE_USER_ROUTE)(*this);
}

////////////////////////////////////////////////////////////////////////////////
crow::response 
DeleteUser::operator()(const crow::request& request, const std::string& name) const
{
	auto db = database(request);
	if (not db)
		return forbidden();
	db.get().delete_user(name);
	return ok();
}
