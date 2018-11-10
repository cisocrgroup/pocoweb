#ifndef pcw_SuspiciousWordsRoute_hpp__
#define pcw_SuspiciousWordsRoute_hpp__

#include "core/CrtpRoute.hpp"
#include <crow/logging.h>
#include <future>
#include <memory>
#include <mutex>
#include <set>

namespace pcw {
struct Token;
class Line;
class Project;
class SuspiciousWordsRoute : public CrtpRoute<SuspiciousWordsRoute>
{
public:
  SuspiciousWordsRoute() = default;
  virtual ~SuspiciousWordsRoute() noexcept = default;

  virtual void Register(App& app) override;
  virtual const char* route() const noexcept override { return route_; }
  virtual const char* name() const noexcept override { return name_; }

  pcw_crtp_route_def_impl__(int);
  pcw_crtp_route_def_impl__(int, int);
  pcw_crtp_route_def_impl__(int, int, int);
  Response impl(HttpGet, const Request& req, int bid) const;
  Response impl(HttpGet, const Request& req, int bid, int pid) const;
  Response impl(HttpGet, const Request& req, int bid, int pid, int lid) const;

private:
  static const char *route_, *name_;
};
}

#endif // pcw_SuspiciousWordsRoute_hpp__
