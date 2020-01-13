#ifndef pcw_Route_hpp__
#define pcw_Route_hpp__

#include "database/mysql.hpp"
#include "utils/Error.hpp"
#include <boost/optional.hpp>
#include <crow/http_request.h>
#include <crow/http_response.h>
#include <memory>
#include <vector>

namespace crow {
template <typename... Middleware> class Crow;
}

namespace pcw {
class Database;
struct Config;
using ConfigPtr = std::shared_ptr<Config>;
class Route;
using RoutePtr = std::unique_ptr<Route>;
class Project;
using ProjectPtr = std::shared_ptr<Project>;
struct AppCache;
using CachePtr = std::shared_ptr<AppCache>;
class Project;
using ProjectPtr = std::shared_ptr<Project>;
class Page;
using PagePtr = std::shared_ptr<Page>;
class Line;
using LinePtr = std::shared_ptr<Line>;
class Route;

class Route {
public:
  using Response = crow::response;
  using Request = crow::request;
  using RoutingParams = crow::routing_params;

  static Response ok() noexcept { return Response(200); }
  static Response created() noexcept { return Response(201); }
  static Response accepted() noexcept { return Response(202); }
  static Response bad_request() noexcept { return Response(400); }
  static Response forbidden() noexcept { return Response(403); }
  static Response not_found() noexcept { return Response(404); }
  static Response conflict() noexcept { return Response(409); }
  static Response internal_server_error() noexcept { return Response(500); }
  static Response not_implemented() noexcept { return Response(501); }
  static Response service_not_available() { return Response(503); }

  using App = crow::Crow<>;
  virtual ~Route() noexcept = default;
  virtual void Register(App &) = 0;
  virtual const char *route() const noexcept = 0;
  virtual const char *name() const noexcept = 0;
  virtual void Deregister(App &) {}
  void set_cache(CachePtr cache) noexcept { cache_ = std::move(cache); }
  void set_config(ConfigPtr c) noexcept { config_ = std::move(c); }
  void set_connection_pool(MysqlConnectionPoolSptr pool) noexcept {
    connection_pool_ = std::move(pool);
  }
  MysqlConnectionPoolSptr get_connection_pool() const noexcept {
    return connection_pool_;
  }

protected:
  const Config &get_config() const noexcept { return *config_; }
  MysqlConnection get_connection() const;
  MysqlConnection must_get_connection() const;

  static std::string extract_content(const crow::request &request);

private:
  static std::string extract_multipart(const Request &req,
                                       const std::string &boundary);
  static std::string extract_raw(const Request &req);

  MysqlConnectionPoolSptr connection_pool_;
  CachePtr cache_;
  ConfigPtr config_;
};

class Routes : private std::vector<RoutePtr> {
public:
  using Base = std::vector<RoutePtr>;
  using Base::back;
  using Base::begin;
  using Base::clear;
  using Base::end;
  using Base::push_back;
  using Base::value_type;
};
} // namespace pcw

#endif // pcw_Route_hpp__
