#include <boost/algorithm/string/predicate.hpp>
#include <crow.h>
#include "database/NewDatabase.hpp"
#include "core/Session.hpp"
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
	auto conn = connection();
	auto session = this->session(request);
	assert(conn);
	assert(session);
	SessionLock lock(*session);

	auto user = session->find_user(conn, name);
	if (not user)
		return not_found();

	// update user information
	update(*user, key, val);
	MysqlCommiter commiter(conn);
	update_user(conn.db(), *user);
	commiter.commit();
	return ok();
}

////////////////////////////////////////////////////////////////////////////////
void
UpdateUser::update(User& user, const std::string& key, const std::string& val) const noexcept
{
	static const std::string institute = "institute";
	static const std::string email = "email";

	CROW_LOG_INFO << "(UpdateUser) setting " << key << " = " << val;
	// check which field to set
	if (boost::iequals(key, institute)) {
		user.institute = val;
	} else if (boost::iequals(key, email)) {
		user.email = val;
	} else {
		THROW(BadRequest, "bad user setting: ", key, "=", val);
	}
}
