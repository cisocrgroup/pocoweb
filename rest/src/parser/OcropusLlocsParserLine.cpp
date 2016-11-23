#include <fstream>
#include <iostream>
#include <regex>
#include "core/Error.hpp"
#include "core/Line.hpp"
#include "OcropusLlocsParserLine.hpp"

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
OcropusLlocsParserLine::OcropusLlocsParserLine(int id, Path llocs, const Path& img)
	: line_(std::make_shared<Line>(id))
	, llocs_(std::move(llocs))
{
	line_->img = img;
	init();
}

////////////////////////////////////////////////////////////////////////////////
LinePtr
OcropusLlocsParserLine::line(int id) const
{
	return line_;
}

////////////////////////////////////////////////////////////////////////////////
std::wstring
OcropusLlocsParserLine::wstring() const
{
	return line_->wcor();
}

////////////////////////////////////////////////////////////////////////////////
std::string
OcropusLlocsParserLine::string() const
{
	return line_->cor();
}

////////////////////////////////////////////////////////////////////////////////
void
OcropusLlocsParserLine::insert(size_t pos, wchar_t c)
{
	line_->insert(pos, c);
}

////////////////////////////////////////////////////////////////////////////////
void
OcropusLlocsParserLine::erase(size_t pos)
{
	line_->erase(pos);
}

////////////////////////////////////////////////////////////////////////////////
void
OcropusLlocsParserLine::set(size_t pos, wchar_t c)
{
	line_->set(pos, c);
}

////////////////////////////////////////////////////////////////////////////////
void
OcropusLlocsParserLine::init()
{
	static const std::wregex linere{LR"((.?)\t(-?\d*\.?\d+)(\t(\d*\.?\d+))?)"};
	std::wifstream is(llocs_.string());
	if (not is.good())
		throw std::system_error(errno, std::system_category(), llocs_.string());
	std::wstring tmp;
	std::wsmatch m;
	while (std::getline(is, tmp)) {
		if (not std::regex_match(tmp, m, linere))
			THROW(BadRequest, "(OcropusLLocsParserLine) Invalid llocs file: ", llocs_);
		wchar_t c = m[1].length() ? *m[1].first : L' ';
		double cut = std::stod(m[2]);
		double conf = m[4].length() ? std::stod(m[4]) : 0;
		if (cut < 0) // fix cuts smaller than 0 (does happen)
			cut = 0;
		line_->append(c, cut, conf);
	}
}

