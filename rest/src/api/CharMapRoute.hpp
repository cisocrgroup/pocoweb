#ifndef pcw_CharMapRoute_hpp__
#define pcw_CharMapRoute_hpp__

#include "core/CrtpRoute.hpp"

namespace pcw {
class CharMapRoute : public CrtpRoute<CharMapRoute> {
public:
  virtual ~CharMapRoute() noexcept override = default;
  virtual void Register(App &app) override;
  virtual const char *route() const noexcept override { return route_; }
  virtual const char *name() const noexcept override { return name_; }

  pcw_crtp_route_def_impl__(int);
  Response impl(HttpGet, const Request &req, int bid) const;

private:
  static const char *route_;
  static const char *name_;
};
} // namespace pcw

#endif // pcw_CharMapRoute_hpp__
