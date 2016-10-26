#include <crow.h>
#include <cppconn/connection.h>
#include "db.hpp"
#include "Cache.hpp"
#include "AppCache.hpp"
#include "Config.hpp"
#include "Sessions.hpp"
#include "App.hpp"

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
App::App(const char *config)
	: routes_()
	, plugins_()
	, app_(std::make_unique<pcw::Route::App>())
	, cache_(std::make_shared<AppCache>(100, 100))
	, config_(std::make_shared<Config>(Config::load(config)))
	, sessions_(std::make_shared<Sessions>(*config_))
{
}

////////////////////////////////////////////////////////////////////////////////
App::~App() noexcept
{
	stop();
}

////////////////////////////////////////////////////////////////////////////////
void
App::run()
{
	assert(config_);
	assert(app_);
	CROW_LOG_INFO << "(App) Starting server";
	app_->port(config_->daemon.port)
		.concurrency(config_->daemon.threads)
		.bindaddr(config_->daemon.host)
		.run();
}

////////////////////////////////////////////////////////////////////////////////
void
App::stop() noexcept
{
	try {
		// order matters here: first delete the server, 
		// then delete all routes and then close the plugins
		app_->stop();
		app_.release();
		routes_.clear();
		plugins_.clear();
	} catch (const std::exception& e) {
		CROW_LOG_ERROR << "(App) Error: " << e.what();
	}
}

////////////////////////////////////////////////////////////////////////////////
void 
App::Register(RoutePtr route)
{
	assert(app_);
	if (route) {
		try {
			assert(sessions_);
			assert(config_);
			assert(cache_);
			route->set_sessions(sessions_);
			route->set_config(config_);
			route->set_cache(cache_);
			route->Register(*app_);
			routes_.push_back(std::move(route));
			CROW_LOG_INFO << "(App) Registered route " << routes_.back()->name() 
				      << ": " << routes_.back()->route() 	
				      << " [" << routes_.back().get() << "]";
		} catch (const std::exception& e) {
			CROW_LOG_ERROR << "(App) Could not register: " << e.what();
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
			CROW_LOG_INFO << "(App) Registering plugin " << p.first << ": " << path;
			pcw::Plugin plugin(path);
			plugin(p.first, *this);
			plugins_.push_back(std::move(plugin));
		} catch (const std::exception& e) {
			// exception includes path of the plugin in its error message
			CROW_LOG_ERROR << "(App) Unable to register plugin: " << e.what();
		}
	}
}
