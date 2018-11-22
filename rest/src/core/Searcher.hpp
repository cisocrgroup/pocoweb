#ifndef pcw_Searcher_hpp__
#define pcw_Searcher_hpp__

#include "Line.hpp"
#include "Page.hpp"
#include "Project.hpp"
#include <map>
#include <memory>
#include <vector>

namespace pcw {
class Searcher
{
public:
  using ConstLineSptr = std::shared_ptr<const Line>;
  using Matches = std::map<ConstLineSptr, std::vector<Token>>;
  Searcher()
    : Searcher(nullptr)
  {}
  Searcher(const Project& project);

  void set_ignore_case(bool ic = true) { ignore_case_ = ic; }
  bool ignore_case() const noexcept { return ignore_case_; }
  void set_project(const Project& project) noexcept;
  Matches find(const std::wstring& str) const;
  Matches find(const std::string& str) const;

  template<class F>
  Matches find_impl(F f) const;

private:
  Searcher(std::shared_ptr<const Project> p)
    : project_(std::move(p))
    , ignore_case_(true)
  {}
  std::shared_ptr<const Project> project_;
  bool ignore_case_;
};
}

////////////////////////////////////////////////////////////////////////////////
template<class F>
inline pcw::Searcher::Matches
pcw::Searcher::find_impl(F f) const
{
  if (not project_)
    return {};
  Matches matches;
  for (const auto& page : *project_) {
    if (page) {
      for (const auto& line : *page) {
        if (line) {
          line->each_token([&](const auto& t) {
            if (f(t))
              matches[line].push_back(t);
          });
        }
      }
    }
  }
  return matches;
}

#endif // pcw_Searcher_hpp__
