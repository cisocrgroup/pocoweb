#ifndef pcw_App_hpp__
#define pcw_App_hpp__

#include "Route.hpp"
#include "database/mysql.hpp"
#include <memory>
#include <vector>

namespace pcw {
struct Config;
using ConfigPtr = std::shared_ptr<Config>;
struct AppCache;
using CachePtr = std::shared_ptr<AppCache>;

class App {
public:
  App(const char *config);
  App(std::shared_ptr<Config> config);
  ~App() noexcept;
  App(const App &) = delete;
  App &operator=(const App &) = delete;
  App(App &&) = delete;
  App &operator=(App &&) = delete;

  void Register(RoutePtr route);
  const Routes &routes() const noexcept { return routes_; }
  const Config &config() const noexcept { return *config_; }
  const ConfigPtr &config_ptr() const noexcept { return config_; }
  MysqlConnectionPool &connection_pool() const noexcept {
    return *connection_pool_;
  }
  void run();
  void stop() noexcept;
  static int version() noexcept;
  static int version_major() noexcept;
  static int version_minor() noexcept;
  static int version_patch() noexcept;
  static const char *version_str() noexcept;

private:
  void log(const Route &route) const;

  pcw::Routes routes_;
  std::unique_ptr<pcw::Route::App> app_;
  const CachePtr cache_;
  const std::shared_ptr<Config> config_;
  const MysqlConnectionPoolSptr connection_pool_;
};
} // namespace pcw

#endif // pcw_App_hpp__
