#ifndef pcw_SearchRoute_hpp__
#define pcw_SearchRoute_hpp__

#include "core/CrtpRoute.hpp"

namespace pcw {
class Book;
class Project;
using ProjectPtr = std::shared_ptr<Project>;

class SearchRoute : public CrtpRoute<SearchRoute> {
public:
  virtual ~SearchRoute() noexcept override = default;
  virtual void Register(App &app) override;
  virtual const char *route() const noexcept override { return route_; }
  virtual const char *name() const noexcept override { return name_; }

  pcw_crtp_route_def_impl__(int);
  Response impl(HttpGet, const Request &req, int bid) const;

private:
  struct TokenQuery {};
  struct ErrorPatternQuery {};
  Response search(const Request &req, const std::string &q, int bid,
                  TokenQuery) const;
  Response search(const Request &req, const std::string &q, int bid,
                  ErrorPatternQuery) const;
  static const char *route_;
  static const char *name_;
};
} // namespace pcw

#endif // pcw_SearchRoute_hpp__
