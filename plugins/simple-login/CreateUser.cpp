#include <crow.h>
#include "Database.hpp"
#include "CreateUser.hpp"

using namespace pcw;

#define CREATE_USER_ROUTE "/create-user/user<string>/pass/<string>"

////////////////////////////////////////////////////////////////////////////////
const char* CreateUser::route_ = CREATE_USER_ROUTE;

////////////////////////////////////////////////////////////////////////////////
const char* CreateUser::name_ = "CreateUser";

////////////////////////////////////////////////////////////////////////////////
void 
CreateUser::Register(App& app) 
{
	CROW_ROUTE(app, CREATE_USER_ROUTE)(*this);
}

////////////////////////////////////////////////////////////////////////////////
crow::response 
CreateUser::operator()(
	const crow::request& request, 
	const std::string& name, 
	const std::string& pass
) const {

	try {
		auto db = database(request);
		if (not db)
			return forbidden();
		auto user = db.get().insert_user(name, pass);
		if (user)
			return created();
	} catch (const std::exception& e) {
		CROW_LOG_ERROR << "(CreateUser) Error: " << e.what();
	}
	return bad_request();
}
