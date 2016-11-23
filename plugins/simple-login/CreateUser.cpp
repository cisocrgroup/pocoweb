#include <crow.h>
#include "core/Database.hpp"
#include "CreateUser.hpp"

using namespace pcw;

#define CREATE_USER_ROUTE "/create/user/<string>/pass/<string>"

////////////////////////////////////////////////////////////////////////////////
const char* CreateUser::route_ = CREATE_USER_ROUTE;

////////////////////////////////////////////////////////////////////////////////
const char* CreateUser::name_ = "CreateUser";

////////////////////////////////////////////////////////////////////////////////
void
CreateUser::Register(App& app)
{
	CROW_ROUTE(app, CREATE_USER_ROUTE).methods("POST"_method)(*this);
}

////////////////////////////////////////////////////////////////////////////////
crow::response
CreateUser::impl(
	HttpPost,
	const crow::request& request,
	const std::string& name,
	const std::string& pass
) const {
	auto db = database(request);
	auto user = db.insert_user(name, pass);
	if (user)
		return created();
	THROW(BadRequest, "Could not create user ", name);
}
