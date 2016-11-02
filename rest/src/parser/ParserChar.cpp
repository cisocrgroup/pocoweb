#include "ParserChar.hpp"

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
BasicParserChar::BasicParserChar(wchar_t c, double conf, int cut)
	: char_(c)
	, conf_(conf)
	, cut_(cut)
{
}

////////////////////////////////////////////////////////////////////////////////
ParserCharPtr 
BasicParserChar::clone() const 
{
	return std::make_shared<BasicParserChar>(char_, conf_, cut_);
}
