#include "Searcher.hpp"
#include "Project.hpp"
#include <boost/algorithm/string.hpp>
#include <boost/regex.hpp>
#include <core/util.hpp>
#include <crow/logging.h>
#include <utf8.h>

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
Searcher::Searcher(const Project &project)
    : Searcher(project.shared_from_this(), 0, 10) {}

////////////////////////////////////////////////////////////////////////////////
Searcher::Searcher(const Project &project, int skip, int max)
    : Searcher(project.shared_from_this(), skip, max) {}

////////////////////////////////////////////////////////////////////////////////
void Searcher::set_project(const Project &project) noexcept {
  project_ = project.shared_from_this();
}

////////////////////////////////////////////////////////////////////////////////
Searcher::Matches Searcher::find(const std::wstring &str) const {
  return find_impl([&](const auto &token) {
    return ignore_case_ ? boost::iequals(token.wcor(), str)
                        : token.wcor() == str;
  });
}

////////////////////////////////////////////////////////////////////////////////
Searcher::Matches Searcher::find(const std::string &str) const {
  std::wstring wstr;
  wstr.reserve(str.size());
  utf8::utf8to32(begin(str), end(str), std::back_inserter(wstr));
  return find(wstr);
}
