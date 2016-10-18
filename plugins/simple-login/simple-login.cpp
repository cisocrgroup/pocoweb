#include <crow.h>
#include "util.hpp"
#include "App.hpp"
#include "Login.hpp"
#include "CreateUser.hpp"
#include "UpdateUser.hpp"
#include "DeleteUser.hpp"

////////////////////////////////////////////////////////////////////////////////
const char*
do_plugin(const std::string& p, pcw::App& app) noexcept
{
	try {
		CROW_LOG_INFO << "(" << p << ") do_plugin"; 
		auto login = std::make_unique<pcw::Login>();
		CROW_LOG_INFO << "login [" << login.get() << "]";
		CROW_LOG_INFO << "login [" << (void*)login->name() << "]";
		CROW_LOG_INFO << "login " << login->name();
		CROW_LOG_INFO << "login [" << (void*)login->route() << "]";
		CROW_LOG_INFO << "login " << login->route();

		auto create_user = std::make_unique<pcw::CreateUser>();
		CROW_LOG_INFO << "create_user [" << create_user.get() << "]";
		CROW_LOG_INFO << "create_user [" << (void*)create_user->name() << "]";
		CROW_LOG_INFO << "create_user " << create_user->name();
		CROW_LOG_INFO << "create_user [" << (void*)create_user->route() << "]";
		CROW_LOG_INFO << "create_user " << create_user->route();

		auto update_user = std::make_unique<pcw::UpdateUser>();
		CROW_LOG_INFO << "update_user [" << update_user.get() << "]";
		CROW_LOG_INFO << "update_user [" << (void*)update_user->name() << "]";
		CROW_LOG_INFO << "update_user " << update_user->name();
		CROW_LOG_INFO << "update_user [" << (void*)update_user->route() << "]";
		CROW_LOG_INFO << "update_user " << update_user->route();

		auto delete_user = std::make_unique<pcw::DeleteUser>();
		CROW_LOG_INFO << "delete_user [" << delete_user.get() << "]";
		CROW_LOG_INFO << "delete_user [" << (void*)delete_user->name() << "]";
		CROW_LOG_INFO << "delete_user " << delete_user->name();
		CROW_LOG_INFO << "delete_user [" << (void*)delete_user->route() << "]";
		CROW_LOG_INFO << "delete_user " << delete_user->route();

		app.Register(std::move(login));
		app.Register(std::move(create_user));
		app.Register(std::move(update_user));
		app.Register(std::move(delete_user));
		
		// app.Register(std::make_unique<pcw::Login>());
		return nullptr;
	} catch (const std::exception& e) {
		return pcw::what(e);
	}
}

extern "C" {
////////////////////////////////////////////////////////////////////////////////
const char*
plugin(const std::string& p, pcw::App& app)
{
	return do_plugin(p, app);
}
}

