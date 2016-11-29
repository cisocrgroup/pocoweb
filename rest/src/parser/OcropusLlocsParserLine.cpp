#include <fstream>
#include <iostream>
#include <regex>
#include <utf8.h>
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
	std::ifstream is(llocs_.string());
	if (not is.good())
		throw std::system_error(errno, std::system_category(), llocs_.string());
	std::string tmp;
	while (std::getline(is, tmp)) {
		auto line = tmp.data();
		auto first_tab = strchr(line, '\t');
		if (not first_tab)
			THROW(BadRequest, "Invalid llocs line: ", tmp);
		if (line == first_tab) // skip 0 class
			continue;
		auto second_tab = strchr(first_tab + 1, '\t');

		wchar_t c = utf8::next(line, first_tab);
		if (c == 0) // skip 0 class
			continue;
		double cut = std::strtod(first_tab + 1, nullptr);
		double conf = 0;

		if (cut < 0) // fix cuts smaller than 0 (does happen)
			cut = 0;
		if (second_tab) // check if the llocs file contains confidence values
			conf = std::strtod(second_tab + 1, nullptr);
		line_->append(c, cut, conf);
	}
	is.close();
}

