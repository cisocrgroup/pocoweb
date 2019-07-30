#ifndef pcw_SplitRoute_hpp__
#define pcw_SplitRoute_hpp__

#include "core/CrtpRoute.hpp"
namespace crow {
namespace json {
class wvalue;
} // namespace json
} // namespace crow
namespace pcw {
class Book;
class Project;
using ProjectPtr = std::shared_ptr<Project>;
using Json = crow::json::wvalue;

class SplitRoute : public CrtpRoute<SplitRoute> {
public:
  virtual ~SplitRoute() noexcept override = default;
  virtual void Register(App &app) override;
  virtual const char *route() const noexcept override { return route_; }
  virtual const char *name() const noexcept override { return name_; }

  pcw_crtp_route_def_impl__(int);
  Response impl(HttpPost, const Request &req, int bid) const;

private:
  static const char *route_;
  static const char *name_;
};

class Splitter {
public:
  Splitter(MysqlConnection &c, int pid, bool random)
      : c_(c), pid_(pid), owner_(), random_(random) {}
  std::unordered_map<int, std::vector<int>> split(const std::vector<int> &ids);
  Json toJSON(const std::unordered_map<int, std::vector<int>> &projs) const;

private:
  std::vector<std::vector<int>> genProjects(size_t n) const;
  std::unordered_map<int, std::vector<int>>
  assign(const std::vector<int> &ids,
         const std::vector<std::vector<int>> &projs) const;
  MysqlConnection &c_;
  int pid_, owner_;
  bool random_;
};
} // namespace pcw
#endif // pcw_SplitRoute_hpp__
