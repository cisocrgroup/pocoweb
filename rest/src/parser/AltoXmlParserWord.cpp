#include <cassert>
#include <cstring>
#include <utf8.h>
#include "ParserChar.hpp"
#include "AltoXmlParserWord.hpp"

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
AltoXmlParserWord::AltoXmlParserWord(const pugi::xml_node& node, Chars& chars)
	: node_(node)
{
	make(chars);
}

////////////////////////////////////////////////////////////////////////////////
void
AltoXmlParserWord::update()
{
}

////////////////////////////////////////////////////////////////////////////////
void
AltoXmlParserWord::remove()
{
}

////////////////////////////////////////////////////////////////////////////////
void
AltoXmlParserWord::make(Chars& chars)
{
	auto conf = node_.attribute("WC").as_double();
	auto cont = node_.attribute("CONTENT").value();
	auto x0 = node_.attribute("HPOS").as_int();
	auto y0 = node_.attribute("VPOS").as_int();
	auto w = node_.attribute("WIDTH").as_int();
	auto h = node_.attribute("HEIGHT").as_int();
	Box box{x0, y0, x0 + w, y0 + h};
	std::wstring str;
	utf8::utf8to32(cont, cont + strlen(cont), std::back_inserter(str));
	auto boxes = box.split(static_cast<int>(str.size()));
	assert(boxes.size() == str.size());
	for (auto i = 0U; i < str.size(); ++i) {
		auto c = std::make_shared<ParserWordChar>(
			boxes[i],
			str[i],
			conf,
			shared_from_this()
		);
		chars.push_back(c);
	}
}
