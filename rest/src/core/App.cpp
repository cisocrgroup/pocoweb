#include "App.hpp"
#include "AppCache.hpp"
#include "Cache.hpp"
#include "Config.hpp"
#include "SessionStore.hpp"
#include <crow.h>

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
App::App(const char* config)
  : routes_()
  , plugins_()
  , app_()
  , cache_(std::make_shared<AppCache>(100, 100, 100))
  , config_(std::make_shared<Config>(Config::load(config)))
  , session_store_(std::make_shared<SessionStore>())
  , connection_pool_(std::make_shared<MysqlConnectionPool>(
      config_->db.connections,
      mysqlConnectionConfigFromConfig(*config_)))
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
  if (not app_)
    app_ = std::make_unique<Route::App>();
  CROW_LOG_DEBUG << "(App) Starting server version " << App::version_str()
                 << " on " << config_->daemon.host << ":"
                 << config_->daemon.port;
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
    if (not app_)
      app_ = std::make_unique<Route::App>();
    try {
      assert(session_store_);
      assert(connection_pool_);
      assert(config_);
      assert(cache_);
      route->set_session_store(session_store_);
      route->set_config(config_);
      route->set_cache(cache_);
      route->set_connection_pool(connection_pool_);
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
    const char* e = strchr(i, ',');
    std::string r;
    if (e) {
      r.assign(i, e);
      i = e + 1;
    } else {
      r.assign(i);
      i = e;
    }
    CROW_LOG_DEBUG << "(App) Registered route " << route.name() << ": " << r
                   << " [" << &route << "]";
  }
}

////////////////////////////////////////////////////////////////////////////////
void
App::register_plugins()
{
  assert(config_);
  if (not app_)
    app_ = std::make_unique<Route::App>();
  for (const auto& p : config_->plugins.configs) {
    auto path = p.second.get<std::string>("path");
    CROW_LOG_DEBUG << "(App) Registering plugin " << p.first << ": " << path;
    pcw::Plugin plugin(path);
    plugin(p.first, *this);
    plugins_.push_back(std::move(plugin));
  }
}

////////////////////////////////////////////////////////////////////////////////
int
App::version() noexcept
{
  return (100 * 100 * PCW_API_VERSION_MAJOR) + (100 * PCW_API_VERSION_MINOR) +
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
#define QQUOTE(x) #x
#define QUOTE(x) QQUOTE(x)

#define PCW_API_VERSION_STR                                                    \
  QUOTE(PCW_API_VERSION_MAJOR)                                                 \
  "." QUOTE(PCW_API_VERSION_MINOR) "." QUOTE(PCW_API_VERSION_PATCH)
  return PCW_API_VERSION_STR;
#undef QQUOTE
#undef QUOTE
#undef PCW_API_VERSION_STR
}
