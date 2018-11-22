#ifndef pcw_SplitImagesRoute_hpp__
#define pcw_SplitImagesRoute_hpp__

#include "core/CrtpRoute.hpp"

namespace crow {
namespace json {
class rvalue;
}
}

namespace pcw {
class WagnerFischer;
struct Token;

class SplitImagesRoute : public CrtpRoute<SplitImagesRoute>
{
public:
  virtual ~SplitImagesRoute() noexcept override = default;
  virtual void Register(App& app) override;
  virtual const char* route() const noexcept override { return route_; }
  virtual const char* name() const noexcept override { return name_; }

  pcw_crtp_route_def_impl__(int, int, int, int);
  Response impl(HttpGet, const Request& req, int bid, int pid, int lid, int tid)
    const;

private:
  Response create_split_images(const Session& session,
                               const Line& line,
                               int tokenid) const;

  static const char* route_;
  static const char* name_;
};
}

#endif // pcw_SplitImagesRoute_hpp__
