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
	, app_()
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
	CROW_LOG_INFO << "(App) Starting server";
	app_ = std::make_unique<Route::App>();
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
		if (app_) {
			app_->stop();
			app_.reset();
		}
		routes_.clear();
		plugins_.clear();
	} catch (const std::exception& e) {
		CROW_LOG_ERROR << "(App) Error: " << e.what();
	} catch (...) {
		CROW_LOG_ERROR << "(App) Unknown error";
	}
}

////////////////////////////////////////////////////////////////////////////////
void
App::Register(RoutePtr route)
{
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
			log(*routes_.back());
		} catch (const std::exception& e) {
			CROW_LOG_ERROR << "(App) Could not register: " << e.what();
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
void
App::log(const Route& route) const
{
	for (auto i = route.route(); i;) {
		auto e = strchr(i, ',');
		std::string r;
		if (e) {
			r.assign(i, e);
			i = e + 1;
		} else {
			r.assign(i);
			i = e;
		}
		CROW_LOG_INFO << "(App) Registered route "
			      << route.name()
			      << ": " << r
			      << " [" << &route << "]";
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
			CROW_LOG_INFO << "(App) Registering plugin "
				      << p.first << ": " << path;
			pcw::Plugin plugin(path);
			plugin(p.first, *this);
			plugins_.push_back(std::move(plugin));
		} catch (const std::exception& e) {
			// exception includes path of the plugin in its error message
			CROW_LOG_ERROR << "(App) Unable to register plugin: " << e.what();
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
int
App::version() noexcept
{
	return (100 * 100 * PCW_API_VERSION_MAJOR) +
		(100 * PCW_API_VERSION_MINOR) +
		(1 * PCW_API_VERSION_PATCH);
}

////////////////////////////////////////////////////////////////////////////////
int
App::version_major() noexcept
{
	return PCW_API_VERSION_MAJOR;
}

////////////////////////////////////////////////////////////////////////////////
int
App::version_minor() noexcept
{
	return PCW_API_VERSION_MINOR;
}

////////////////////////////////////////////////////////////////////////////////
int
App::version_patch() noexcept
{
	return PCW_API_VERSION_PATCH;
}

////////////////////////////////////////////////////////////////////////////////
const char*
App::version_str() noexcept
{
#	define QQUOTE(x) #x
#	define QUOTE(x) QQUOTE(x)

#	define PCW_API_VERSION_STR QUOTE(PCW_API_VERSION_MAJOR) "." \
		QUOTE(PCW_API_VERSION_MINOR) "." \
		QUOTE(PCW_API_VERSION_PATCH)
	return PCW_API_VERSION_STR;
#	undef QQUOTE
#	undef QUOTE
#	undef PCW_API_VERSION_STR
}
