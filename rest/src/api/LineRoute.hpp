#ifndef pcw_LineRoute_hpp__
#define pcw_LineRoute_hpp__

#include "core/CrtpRoute.hpp"
#include "database/CorType.hpp"

namespace crow {
namespace json {
class wvalue;
class rvalue;
} // namespace json
} // namespace crow
namespace pcw {
struct Token;
struct DbLine;
class Line;
using Json = crow::json::wvalue;
using RJson = crow::json::rvalue;
class LineRoute : public CrtpRoute<LineRoute> {
public:
  virtual ~LineRoute() noexcept override = default;
  virtual void Register(App &app) override;
  virtual const char *route() const noexcept override { return route_; }
  virtual const char *name() const noexcept override { return name_; }

  pcw_crtp_route_def_impl__(int, int, int);
  pcw_crtp_route_def_impl__(int, int, int, int);
  Response impl(HttpGet, const Request &req, int bid, int pid, int lid) const;
  Response impl(HttpPut, const Request &req, int bid, int pid, int lid) const;
  Response impl(HttpPost, const Request &req, int bid, int pid, int lid) const;
  Response impl(HttpDelete, const Request &req, int pid, int p, int lid) const;
  Response impl(HttpGet, const Request &req, int pid, int p, int lid,
                int tid) const;
  Response impl(HttpPut, const Request &req, int pid, int p, int lid,
                int tid) const;
  Response impl(HttpPost, const Request &req, int pid, int p, int lid,
                int tid) const;

private:
  static CorType getCorType(const RJson &json);
  static void correct(DbLine &line, const std::string &correction,
                      CorType type);
  static void correct(DbLine &line, const std::string &correction, int begin,
                      int len, CorType type);
  static void update(MysqlConnection &conn, const DbLine &line);
  static const char *route_;
  static const char *name_;
};
} // namespace pcw

#endif // pcw_LineRoute_hpp__
