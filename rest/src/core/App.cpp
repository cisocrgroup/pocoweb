#include "App.hpp"
#include "AppCache.hpp"
#include "Cache.hpp"
#include "Config.hpp"
#include "SessionStore.hpp"
#include <crow.h>

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
App::App(const char *config)
    : App(std::make_shared<Config>(Config::load(config))) {}

////////////////////////////////////////////////////////////////////////////////
App::App(std::shared_ptr<Config> config)
    : routes_(), app_(), cache_(std::make_shared<AppCache>(100, 100)),
      config_(std::move(config)),
      session_store_(std::make_shared<SessionStore>()),
      connection_pool_(std::make_shared<MysqlConnectionPool>(
          config_->db.connections, mysqlConnectionConfigFromConfig(*config_))) {
}

////////////////////////////////////////////////////////////////////////////////
App::~App() noexcept { stop(); }

////////////////////////////////////////////////////////////////////////////////
void App::run() {
  assert(config_);
  if (not app_) {
    app_ = std::make_unique<Route::App>();
  }
  CROW_LOG_INFO << "(App) Starting server version " << App::version_str()
                << " on " << config_->daemon.host;
  auto port = 80;
  auto host = config_->daemon.host;
  const auto pos = config_->daemon.host.find(":");
  if (pos != std::string::npos) {
    host = config_->daemon.host.substr(0, pos);
    port = std::atoi(config_->daemon.host.substr(pos + 1).data());
  }
  app_->concurrency(std::thread::hardware_concurrency())
      .bindaddr(host)
      .port(port)
      .run();
  CROW_LOG_INFO << "(App) Started server version " << App::version_str()
                << " on " << config_->daemon.host;
}

////////////////////////////////////////////////////////////////////////////////
void App::stop() noexcept {
  try {
    // Order matters here: first delete the server, then delete all routes.
    if (app_) {
      app_.reset();
    }
    routes_.clear();
  } catch (const std::exception &e) {
    CROW_LOG_ERROR << "(App) Error: " << e.what();
  } catch (...) {
    CROW_LOG_ERROR << "(App) Unknown error";
  }
}

////////////////////////////////////////////////////////////////////////////////
void App::Register(RoutePtr route) {
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
    } catch (const std::exception &e) {
      CROW_LOG_ERROR << "(App) Could not register: " << e.what();
    }
  }
}

////////////////////////////////////////////////////////////////////////////////
void App::log(const Route &route) const {
  for (auto i = route.route(); i;) {
    const char *e = strchr(i, ',');
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
const char *App::version_str() noexcept {
#define QQUOTE(x) #x
#define QUOTE(x) QQUOTE(x)
  return QUOTE(PCW_API_VERSION);
}
