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
		auto login = std::make_unique<pcw::Login>();
		auto create_user = std::make_unique<pcw::CreateUser>();
		auto update_user = std::make_unique<pcw::UpdateUser>();
		auto delete_user = std::make_unique<pcw::DeleteUser>();

		app.Register(std::move(login));
		app.Register(std::move(create_user));
		app.Register(std::move(update_user));
		app.Register(std::move(delete_user));
		
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

