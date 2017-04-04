#include <regex>
#include <utf8.h>
#include "Project.hpp"
#include "Searcher.hpp"

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
Searcher::Searcher(Project& project): Searcher(project.shared_from_this()) {}

////////////////////////////////////////////////////////////////////////////////
void
Searcher::set_project(Project& project) noexcept
{
	project_ = project.shared_from_this();
}

////////////////////////////////////////////////////////////////////////////////
std::vector<LineSptr>
Searcher::find(const std::wstring& str) const
{
	auto flags = std::regex::optimize | std::regex::nosubs;
	if (ignore_case_)
		flags |= std::regex::icase;
	std::wregex re;
	if (match_words_)
		re = std::wregex(LR"(\b)" + str + LR"(\b)", flags);
	else
		re = std::wregex(str, flags);
	return find([&re](const auto& line) {
		return std::regex_search(line.wcor(), re);
	});
}

////////////////////////////////////////////////////////////////////////////////
std::vector<LineSptr>
Searcher::find(const std::string& str) const
{
	std::wstring wstr;
	wstr.reserve(str.size());
	utf8::utf8to32(begin(str), end(str), std::back_inserter(wstr));
	return find(wstr);
}

