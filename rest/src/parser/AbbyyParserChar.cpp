#include <cassert>
#include <utf8.h>
#include <cstring>
#include "AbbyyParserChar.hpp"

using namespace pcw;

#define streq(a, b) (strcmp(a, b) == 0)

////////////////////////////////////////////////////////////////////////////////
AbbyyParserChar::AbbyyParserChar(const pugi::xml_node& node)
	: ParserChar(
		get_box_from_node(node),
		get_char_from_node(node),
		get_conf_from_node(node)
	)
	, node_(node)
{
	if (not streq(node_.name(), "charParams"))
		throw std::logic_error(
			"(AbbyyParserChar) Not a valid node: " + 
			std::string(node_.name())
		);
}

////////////////////////////////////////////////////////////////////////////////
void
AbbyyParserChar::set(wchar_t c)
{
	assert(node_);
	set_char_to_node(node_, c);
	char_ = c;
}

////////////////////////////////////////////////////////////////////////////////
void
AbbyyParserChar::remove()
{
	assert(node_);
	auto parent = node_.parent();
	parent.remove_child(node_);
	node_ = pugi::xml_node();
}

////////////////////////////////////////////////////////////////////////////////
ParserCharPtr
AbbyyParserChar::clone() 
{
	assert(node_);
	auto parent = node_.parent();
	auto copy = parent.insert_copy_before(node_, node_);
	auto res = std::make_shared<AbbyyParserChar>(copy);
	auto splits = box_.split(2);
	box_ = splits[1];
	res->box_ = splits[0];
	set_box_to_node(node_, box_);
	set_box_to_node(res->node_, res->box_);
	return res;
}

////////////////////////////////////////////////////////////////////////////////
double 
AbbyyParserChar::get_conf_from_node(const pugi::xml_node& node)
{
	return node.attribute("charConfidence").as_double() / 100.0;
}

////////////////////////////////////////////////////////////////////////////////
Box 
AbbyyParserChar::get_box_from_node(const pugi::xml_node& node)
{
	return {
		node.attribute("l").as_int(),
		node.attribute("t").as_int(),
		node.attribute("r").as_int(),
		node.attribute("b").as_int()
	};
}

////////////////////////////////////////////////////////////////////////////////
void 
AbbyyParserChar::set_box_to_node(pugi::xml_node& node, const Box& box)
{
	node.attribute("l").set_value(box.left());
	node.attribute("t").set_value(box.top());
	node.attribute("r").set_value(box.right());
	node.attribute("b").set_value(box.bottom());
}

////////////////////////////////////////////////////////////////////////////////
wchar_t 
AbbyyParserChar::get_char_from_node(const pugi::xml_node& node)
{
	assert(node);
	if (strlen(node.child_value()) == 0 or not node.first_child()) {
		return L' ';	
	} else {
		auto val = node.child_value();
		return utf8::next(val, val + 5); // should be save
	}	
}

////////////////////////////////////////////////////////////////////////////////
void 
AbbyyParserChar::set_char_to_node(pugi::xml_node& node, wchar_t c)
{
	assert(node);
	if (not node.first_child()) {
		node.append_child(pugi::node_pcdata);
	}
	char buf[] = {0,0,0,0,0};		
	utf8::utf32to8(&c, &c + 1, buf);
	node.first_child().set_value(buf);
	assert(get_char_from_node(node) == c);
}

#undef streq
