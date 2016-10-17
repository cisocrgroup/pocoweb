#include <crow.h>
#include <cppconn/connection.h>
#include "db.hpp"
#include "Plugin.hpp"
#include "Config.hpp"
#include "Sessions.hpp"
#include "App.hpp"

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
App::App(const char *config)
	: app_()
	, routes_()
	, config_(std::make_shared<Config>(Config::load(config)))
	, sessions_(std::make_shared<Sessions>(*config_))
{
}

////////////////////////////////////////////////////////////////////////////////
void
App::run()
{
	assert(config_);
	app_.port(config_->daemon.port).concurrency(config_->daemon.threads).run();
}

////////////////////////////////////////////////////////////////////////////////
void 
App::Register(RoutePtr route)
{
	if (route) {
		try {
			assert(sessions_);
			assert(config_);
			route->set_sessions(sessions_);
			route->set_config(config_);
			route->Register(app_);
			routes_.push_back(std::move(route));
			CROW_LOG_INFO << "Registered route " << routes_.back()->name() 
				      << ": " << routes_.back()->route();
		} catch (const std::exception& e) {
			CROW_LOG_ERROR << "Could not register: " << e.what();
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
void
App::register_plugins()
{
	assert(config_);
	for (const auto& p: config_->plugins.configs) {
		try {
			auto path = p.second.get<std::string>("path");
			CROW_LOG_INFO << "Registering plugin " << p.first << ": " << path;
			pcw::Plugin plugin(path);
			plugin(p.first, *this);
		} catch (const std::exception& e) {
			// exception includes path of the plugin in its error message
			CROW_LOG_ERROR << "Unable to register plugin: " << e.what();
		}
	}
}
