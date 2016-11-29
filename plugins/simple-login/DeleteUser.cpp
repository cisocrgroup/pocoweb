#include <crow.h>
#include <cppconn/connection.h>
#include "core/User.hpp"
#include "core/Sessions.hpp"
#include "core/Database.hpp"
#include "DeleteUser.hpp"

using namespace pcw;

#define DELETE_USER_ROUTE "/delete/user/<string>"

////////////////////////////////////////////////////////////////////////////////
const char* DeleteUser::route_ = DELETE_USER_ROUTE;

////////////////////////////////////////////////////////////////////////////////
const char* DeleteUser::name_ = "DeleteUser";

////////////////////////////////////////////////////////////////////////////////
void
DeleteUser::Register(App& app)
{
	CROW_ROUTE(app, DELETE_USER_ROUTE).methods("DELETE"_method)(*this);
}

////////////////////////////////////////////////////////////////////////////////
Route::Response
DeleteUser::operator()(const Request& req, const std::string& name) const
{
	auto db = database(req);
	db.delete_user(name);
	return ok();
}
