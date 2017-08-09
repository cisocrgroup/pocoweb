#include "DeleteUser.hpp"
#include <crow.h>
#include "core/Session.hpp"
#include "core/User.hpp"
#include "database/Database.hpp"

using namespace pcw;

#define DELETE_USER_ROUTE "/delete/user/<string>"

////////////////////////////////////////////////////////////////////////////////
const char* DeleteUser::route_ = DELETE_USER_ROUTE;

////////////////////////////////////////////////////////////////////////////////
const char* DeleteUser::name_ = "DeleteUser";

////////////////////////////////////////////////////////////////////////////////
void DeleteUser::Register(App& app) {
	CROW_ROUTE(app, DELETE_USER_ROUTE).methods("DELETE"_method)(*this);
}

////////////////////////////////////////////////////////////////////////////////
Route::Response DeleteUser::operator()(const Request& req,
				       const std::string& name) const {
	LockedSession session(must_find_session(req));
	auto conn = must_get_connection();
	delete_user(conn.db(), name);
	return ok();
}
