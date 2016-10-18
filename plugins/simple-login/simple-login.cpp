#include <crow.h>
#include <cppconn/connection.h>
#include "util.hpp"
#include "App.hpp"
#include "Config.hpp"
#include "Login.hpp"
#include "Sessions.hpp"
#include "Database.hpp"
#include "CreateUser.hpp"
#include "UpdateUser.hpp"
#include "DeleteUser.hpp"
#include "User.hpp"

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
static void
insert_default_user(const std::string& p, const App& app)
{
	// this could be inproved.
	// but there is currently no other way to do this easily.
	auto session = std::make_shared<Session>("simple-login-sid");

	Database db(session, app.config_ptr());
	auto name = app.config().plugins[p].get<std::string>("default-user");	
	auto pass = app.config().plugins[p].get<std::string>("default-pass");	
	auto email = app.config().plugins[p].get<std::string>("default-email");	
	auto inst = app.config().plugins[p].get<std::string>("default-institute");	

	auto user = db.select_user(name);
	if (not user)
		user = db.insert_user(name, pass);
	if (not user)
		throw std::runtime_error("(simple-login) could not create default user");
	user->email = email;
	user->institute = inst;
	db.update_user(*user);
}

////////////////////////////////////////////////////////////////////////////////
static const char*
do_plugin(const std::string& p, App& app) noexcept
{
	try {
		insert_default_user(p, app);

		auto login = std::make_unique<Login>();
		auto create_user = std::make_unique<CreateUser>();
		auto update_user = std::make_unique<UpdateUser>();
		auto delete_user = std::make_unique<DeleteUser>();

		app.Register(std::move(login));
		app.Register(std::move(create_user));
		app.Register(std::move(update_user));
		app.Register(std::move(delete_user));
		
		return nullptr;
	} catch (const std::exception& e) {
		return what(e);
	}
}

extern "C" {
////////////////////////////////////////////////////////////////////////////////
const char*
plugin(const std::string& p, App& app)
{
	return do_plugin(p, app);
}
}

