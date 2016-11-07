#include <algorithm>
#include <cassert>
#include <cstring>
#include <utf8.h>
#include "ParserChar.hpp"
#include "ParserPage.hpp"
#include "AltoXmlPageParser.hpp"
#include "AltoXmlParserWord.hpp"

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
AltoXmlParserWord::AltoXmlParserWord(const pugi::xml_node& node)
	: node_(node)
{
	if (strcmp(node.name(), "String") != 0) 
		throw std::logic_error(
			"(AltoXmlParserWord) Invalid node for ALTO word: " +
			std::string(node.name())
		);
	auto x0 = node_.attribute("HPOS").as_int();
	auto y0 = node_.attribute("VPOS").as_int();
	auto w = node_.attribute("WIDTH").as_int();
	auto h = node_.attribute("HEIGHT").as_int();
	box_ = Box{x0, y0, x0 + w, y0 + h};
	conf_ = node_.attribute("WC").as_double();
}

////////////////////////////////////////////////////////////////////////////////
void
AltoXmlParserWord::update(const std::string& word)
{
	node_.attribute("CONTENT").set_value(word.data());
	node_.attribute("HPOS").set_value(box().left());
	node_.attribute("VPOS").set_value(box().top());
	node_.attribute("WIDTH").set_value(box().width());
	node_.attribute("HEIGHT").set_value(box().height());
}

////////////////////////////////////////////////////////////////////////////////
void
AltoXmlParserWord::remove()
{
	assert(node_);
	auto parent = node_.parent();
	parent.remove_child(node_);
}

////////////////////////////////////////////////////////////////////////////////
void
AltoXmlParserWord::add_chars_to_line(ParserLine& line)
{
	auto cont = node_.attribute("CONTENT").value();
	std::wstring str;
	utf8::utf8to32(cont, cont + strlen(cont), std::back_inserter(str));
	auto boxes = box_.split(static_cast<int>(str.size()));
	assert(boxes.size() == str.size());
	for (auto i = 0U; i < str.size(); ++i) {
		auto c = std::make_shared<ParserWordChar>(
			boxes[i],
			str[i],
			conf_,
			shared_from_this()
		);
		line.chars.push_back(c);
	}
}

////////////////////////////////////////////////////////////////////////////////
ParserWordPtr 
AltoXmlParserWord::create() 
{
	auto copy = node_.insert_copy_after(node_, node_);
	return std::make_shared<AltoXmlParserWord>(copy);	
}
