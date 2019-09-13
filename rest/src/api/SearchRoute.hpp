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
  struct tq { // token queries
    std::vector<std::string> qs;
    int bid, skip, max;
  };
  struct pq { // pattern queries
    std::vector<std::string> qs;
    int bid, skip, max;
  };
  Response search(MysqlConnection &mysql, tq q) const;
  Response search(MysqlConnection &mysql, pq q) const;

  static const char *route_;
  static const char *name_;
};
} // namespace pcw

#endif // pcw_SearchRoute_hpp__
