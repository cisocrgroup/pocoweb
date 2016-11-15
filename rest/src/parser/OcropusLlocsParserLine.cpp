#include <fstream>
#include <iostream>
#include <regex>
#include "core/BadRequest.hpp"
#include "OcropusLlocsParserLine.hpp"

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
OcropusLlocsParserLine::OcropusLlocsParserLine(Path llocs, const Path& img)
	: line_(std::stoi(llocs.filename().string(), nullptr, 16))
	, llocs_(std::move(llocs))
{
	line_.img = img;
	init();
}

////////////////////////////////////////////////////////////////////////////////
Line
OcropusLlocsParserLine::line(int id) const
{
	return line_;
}

////////////////////////////////////////////////////////////////////////////////
std::wstring
OcropusLlocsParserLine::wstring() const
{
	return line_.wstring();
}

////////////////////////////////////////////////////////////////////////////////
std::string
OcropusLlocsParserLine::string() const
{
	return line_.string();
}

////////////////////////////////////////////////////////////////////////////////
void
OcropusLlocsParserLine::insert(size_t pos, wchar_t c)
{
	line_.insert(pos, c);
}

////////////////////////////////////////////////////////////////////////////////
void
OcropusLlocsParserLine::erase(size_t pos)
{
	line_.erase(pos);
}

////////////////////////////////////////////////////////////////////////////////
void
OcropusLlocsParserLine::set(size_t pos, wchar_t c)
{
	line_.set(pos, c);
}

////////////////////////////////////////////////////////////////////////////////
void
OcropusLlocsParserLine::init()
{
	static const std::wregex linere{LR"((.*)\t(-?\d*\.\d+)(\t(\d*\.\d+))?)"};
	std::wifstream is(llocs_.string());
	if (not is.good())
		throw std::system_error(errno, std::system_category(), llocs_.string());
	std::wstring tmp;
	while (std::getline(is, tmp)) {
		std::wsmatch m;
		if (not std::regex_match(tmp, m, linere))
			throw BadRequest(
				"(OcropusLLocsParserLine) Invalid llocs file: " +
				llocs_.string()
			);
		double cut = std::stod(m[2]);
		double conf = 0;
		if (m[4].length())
			conf = std::stod(m[4]);
		if (cut < 0) // fix cuts smaller than 0 (does happen)
			cut = 0;
		line_.append(m[1], 0, static_cast<int>(cut), conf);
	}
}

