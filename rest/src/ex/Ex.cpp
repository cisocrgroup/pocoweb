#include <regex>
#include "Ex.hpp"

static std::pair<size_t, size_t> parseAddress(const std::string& cmd);

///////////////////////////////////////////////////////////////////////////////
pcw::Ex::Ex(const std::string& cmd)
	: address(parseAddress(cmd))
{
}

///////////////////////////////////////////////////////////////////////////////
std::pair<size_t, size_t>
parseAddress(const std::string& cmd)
{
	static const std::regex r(R"(^(\d+),(\d+))");
	std::smatch m;
	if (std::regex_search(cmd, m, r)) {
		const auto f = std::stoul(m[1]);
		const auto t = std::stoul(m[2]);
		if (f > 0 and f < t) 
			return {f - 1, t - (f - 1)};
	}
	return {0, 0};
}
