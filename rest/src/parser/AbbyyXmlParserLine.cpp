#include <iostream>
#include <utf8.h>
#include "core/Line.hpp"
#include "AbbyyXmlParserLine.hpp"

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
AbbyyXmlParserLine::AbbyyXmlParserLine(pugi::xml_node node)
	: chars_()
	, node_(node)
{
	init();
	box = get_box(node_);
}

////////////////////////////////////////////////////////////////////////////////
LinePtr
AbbyyXmlParserLine::line(int id) const
{
	auto line = std::make_shared<Line>(id, box);
	for (const auto& c: chars_) {
		line->append(c.c, c.box.right(), c.conf);
	}
	return line;
}

////////////////////////////////////////////////////////////////////////////////
std::wstring
AbbyyXmlParserLine::wstring() const
{
	std::wstring res(chars_.size(), 0);
	std::transform(begin(chars_), end(chars_), begin(res), [](const auto& c) {
		return c.c;
	});
	return res;
}

////////////////////////////////////////////////////////////////////////////////
std::string
AbbyyXmlParserLine::string() const
{
	std::string res;
	res.reserve(chars_.size());
	std::for_each(begin(chars_), end(chars_), [&res](const auto& c) {
		const auto wc = c.c;
		utf8::utf32to8(&wc, &wc + 1, std::back_inserter(res));
	});
	return res;
}

////////////////////////////////////////////////////////////////////////////////
void
AbbyyXmlParserLine::insert(size_t pos, wchar_t c)
{
	// std::cerr << "(AbbyyXmlParserLine) insert pos: " << pos << " " << c << "\n";
	assert(pos < chars_.size());
	chars_.insert(begin(chars_) + pos, char_before(chars_[pos], c));
}

////////////////////////////////////////////////////////////////////////////////
void
AbbyyXmlParserLine::erase(size_t pos)
{
	// std::cerr << "(AbbyyXmlParserLine) erase pos: " << pos << "\n";
	assert(pos < chars_.size());
	chars_[pos].node.parent().remove_child(chars_[pos].node);
	chars_.erase(begin(chars_) + pos);
}

////////////////////////////////////////////////////////////////////////////////
void
AbbyyXmlParserLine::set(size_t pos, wchar_t c)
{
	// std::cerr << "(AbbyyXmlParserLine) set pos: " << pos << " " << c << "\n";
	assert(pos < chars_.size());
	chars_[pos].c = c;
	set_char(c, chars_[pos].node);
}

////////////////////////////////////////////////////////////////////////////////
void
AbbyyXmlParserLine::init()
{
	auto char_params = node_.select_nodes(".//charParams");
	for (const auto& char_param: char_params) {
		auto node = char_param.node();
		const auto box = get_box(node);
		const auto conf = node.attribute("charConfidence").as_double();
		const auto c = get_char(node);
		chars_.emplace_back(c, node, conf, box);
	}
}

////////////////////////////////////////////////////////////////////////////////
Box
AbbyyXmlParserLine::get_box(const Node& node)
{
	const auto l = node.attribute("l").as_int();
	const auto t = node.attribute("t").as_int();
	const auto r = node.attribute("r").as_int();
	const auto b = node.attribute("b").as_int();
	return Box {l, t, r, b};
}

////////////////////////////////////////////////////////////////////////////////
void
AbbyyXmlParserLine::set_box(const Box& box, Node& node)
{
	if (not node.attribute("l"))
		node.append_attribute("l");
	if (not node.attribute("t"))
		node.append_attribute("t");
	if (not node.attribute("r"))
		node.append_attribute("r");
	if (not node.attribute("b"))
		node.append_attribute("b");
	node.attribute("l").set_value(box.left());
	node.attribute("t").set_value(box.top());
	node.attribute("r").set_value(box.right());
	node.attribute("b").set_value(box.bottom());
}

////////////////////////////////////////////////////////////////////////////////
wchar_t
AbbyyXmlParserLine::get_char(const Node& node)
{
	const auto text = node.child_value();
	const auto n = strlen(text);
	return n > 0 ? utf8::peek_next(text, text + n) : L' ';
}

////////////////////////////////////////////////////////////////////////////////
void
AbbyyXmlParserLine::set_char(wchar_t c, Node& node)
{
	char buf[] = {0, 0, 0, 0, 0}; // 5 are enough
	utf8::utf32to8(&c, &c + 1, buf);
	node.first_child().set_value(buf);
}

////////////////////////////////////////////////////////////////////////////////
AbbyyXmlParserLine::Char
AbbyyXmlParserLine::char_before(Char& c, wchar_t cc)
{
	auto split = c.box.split(2);
	auto copy = c;
	copy.c = cc;
	assert(c.node);
	copy.node = c.node.parent().insert_copy_before(c.node, c.node);
	assert(copy.node);
	copy.box = split[0];
	c.box = split[1];
	set_box(copy.box, copy.node);
	set_char(copy.c, copy.node);
	set_box(c.box, c.node);
	return copy;
}
