#include <crow.h>
#include "util.hpp"
#include "App.hpp"
#include "LoginRoute.hpp"

////////////////////////////////////////////////////////////////////////////////
const char*
do_plugin(const std::string& p, pcw::App& app) noexcept
{
	try {
		CROW_LOG_INFO << "(" << p << ") do_plugin"; 
		app.Register(std::make_unique<pcw::LoginRoute>());
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

