#include <boost/algorithm/string/predicate.hpp>
#include <crow.h>
#include <cppconn/connection.h>
#include "core/Database.hpp"
#include "core/Sessions.hpp"
#include "core/User.hpp"
#include "UpdateUser.hpp"

using namespace pcw;

#define UPDATE_USER_ROUTE "/update/user/<string>/set/<string>/<string>"

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
	const std::string& name,
	const std::string& key,
	const std::string& val
) const {
	// get user
	auto db = database(request);
	auto user = db.select_user(name);
	if (not user)
		return not_found();

	// update given field
	if (not update(*user, key, val))
		return bad_request();

	// update user information
	db.update_user(*user);
	return ok();
}

////////////////////////////////////////////////////////////////////////////////
bool
UpdateUser::update(User& user, const std::string& key, const std::string& val) const noexcept
{
	static const std::string institute = "institute";
	static const std::string email = "email";

	CROW_LOG_INFO << "(UpdateUser) setting " << key << " = " << val;
	// check which field to set
	if (boost::iequals(key, institute)) {
		user.institute = val;
		return true;
	} else if (boost::iequals(key, email)) {
		user.email = val;
		return true;
	} else {
		return false;
	}
}
