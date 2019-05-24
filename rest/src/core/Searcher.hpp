#ifndef pcw_Searcher_hpp__
#define pcw_Searcher_hpp__

#include "Line.hpp"
#include "Page.hpp"
#include "Project.hpp"
#include <map>
#include <memory>
#include <vector>

namespace pcw {
class Searcher {
public:
  using ConstLineSptr = std::shared_ptr<const Line>;
  using Matches = std::map<ConstLineSptr, std::vector<Token>>;
  Searcher() : Searcher(nullptr, 0, 10) {}
  Searcher(const Project &project);
  Searcher(const Project &project, int skip, int max);

  void set_ignore_case(bool ic = true) { ignore_case_ = ic; }
  bool ignore_case() const noexcept { return ignore_case_; }
  void set_project(const Project &project) noexcept;
  Matches find(const std::wstring &str) const;
  Matches find(const std::string &str) const;

  template <class F> Matches find_impl(F f) const;

private:
  Searcher(std::shared_ptr<const Project> p, int skip, int max)
      : project_(std::move(p)), skip_(skip), max_(max), ignore_case_(true) {}
  std::shared_ptr<const Project> project_;
  const int skip_, max_;
  bool ignore_case_;
};
} // namespace pcw

////////////////////////////////////////////////////////////////////////////////
template <class F>
inline pcw::Searcher::Matches pcw::Searcher::find_impl(F f) const {
  if (not project_)
    return {};
  Matches matches;
  auto skip = skip_;
  auto n = max_;
  for (const auto &page : *project_) {
    if (page) {
      for (const auto &line : *page) {
        if (line) {
          line->each_token([&](const auto &t) {
            if (skip > 0) {
              skip--;
              return;
            }
            if (n > 0 and f(t)) {
              matches[line].push_back(t);
              n--;
            }
          });
        }
      }
    }
  }
  return matches;
}

#endif // pcw_Searcher_hpp__
