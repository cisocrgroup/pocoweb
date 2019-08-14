#ifndef pcw_PageRoute_hpp__
#define pcw_PageRoute_hpp__

#include "core/CrtpRoute.hpp"

namespace crow {
namespace json {
class wvalue;
}
} // namespace crow
namespace pcw {
using Json = crow::json::wvalue;

class PageRoute : public CrtpRoute<PageRoute> {
public:
  virtual ~PageRoute() noexcept override = default;
  virtual void Register(App &app) override;
  virtual const char *route() const noexcept override { return route_; }
  virtual const char *name() const noexcept override { return name_; }

  pcw_crtp_route_def_impl__(int);
  pcw_crtp_route_def_impl__(int, int);
  pcw_crtp_route_def_impl__(int, int, int);
  Response impl(HttpGet, const Request &req, int bid, int pid) const;
  Response impl(HttpDelete, const Request &req, int bid, int pid) const;
  Response impl(HttpGet, const Request &req, int bid) const;
  Response impl(HttpGet, const Request &req, int bid, int pid, int val) const;

private:
  Response first(const Request &req, int bid) const;
  Response last(const Request &req, int bid) const;
  Response next(const Request &req, int bid, int pid, int n) const;
  Response prev(const Request &req, int bid, int pid, int n) const;
  static Response print(Json &json, int pid, const Page &page,
                        const Project &project);

  static const char *route_;
  static const char *name_;
};
} // namespace pcw

#endif // pcw_PageRoute_hpp__
