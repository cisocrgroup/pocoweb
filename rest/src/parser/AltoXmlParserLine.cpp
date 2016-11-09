#include <iostream>
#include <utf8.h>
#include "AltoXmlParserLine.hpp"

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
AltoXmlParserLine::AltoXmlParserLine(pugi::xml_node node)
	: chars_()
	, node_(node) 
	, needs_update_(false)
{
	init();
	box = get_box(node_);
}

////////////////////////////////////////////////////////////////////////////////
std::wstring 
AltoXmlParserLine::wstring() const 
{
	std::wstring res(chars_.size(), 0);
	std::transform(begin(chars_), end(chars_), begin(res), [](const auto& c) {
		return c.c;
	});
	return res;
}

////////////////////////////////////////////////////////////////////////////////
std::string 
AltoXmlParserLine::string() const 
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
AltoXmlParserLine::end_correction()
{
	if (not needs_update_)
		return;
	auto last = node_.last_child();
// 	auto copy = node_.parent().insert_copy_before(node_, node_);
// 	for (auto& node: copy.children())
// 		node.parent().remove_child(node);

	const auto b = begin(chars_);
	const auto e = end(chars_);
	for (auto i = b; i != e; ) {
		auto eot = find_end_of_token(i, e);
		switch (i->type) {
		case Type::Space:
			update_space(i, node_);
			break;
		case Type::Hyphen:
			update_hyphen(i, node_);
			break;
		case Type::Char:
			update_char(i, eot, node_);
			break;
		}
		i = eot;	
	}	

	for (auto n = node_.first_child(); n != last;) {
		auto next = n.next_sibling();
		n.parent().remove_child(n);
		n = next;
	}
	last.parent().remove_child(last);
	// node_.parent().remove_child(node_);
	// node_ = copy;
}

////////////////////////////////////////////////////////////////////////////////
void
AltoXmlParserLine::update_space(Iterator i, Node& parent)
{
	assert(i->type == Type::Space);
	if (not i->node) {
		i->node = parent.append_child("SP");
	} else {
		i->node = parent.append_copy(i->node);
		i->node.set_name("SP");
		i->node.remove_attribute("CONTENT");
	}
	
	set_box(i->box, i->node);
	set_conf(i->conf, i->node);
}

////////////////////////////////////////////////////////////////////////////////
void
AltoXmlParserLine::update_hyphen(Iterator i, Node& parent)
{
	assert(i->type == Type::Hyphen);
	if (not i->node) {
		i->node = parent.append_child("HYP");
	} else {
		i->node = parent.append_copy(i->node);
		i->node.set_name("HYP");
		i->node.remove_attribute("CONTENT");
	}
	set_box(i->box, i->node);
	set_conf(i->conf, i->node);
}

////////////////////////////////////////////////////////////////////////////////
void
AltoXmlParserLine::update_char(Iterator b, Iterator e, Node& parent)
{
	assert(b != e);
	Node node;
	if (not b->node) {
		node = parent.append_child("String");
	} else {
		node = parent.append_copy(b->node);
		node.set_name("String");
	}
	std::wstring wstr;
	double conf = 0;
	auto box = b->box;
	std::for_each(b, e, [&node,&wstr,&conf,&box](Char& c) {
		assert(c.type == Type::Char);
		wstr.push_back(c.c);
		box += c.box;
		conf += c.conf;
		c.node = merge(node, c.node);
		
	});
	conf /= wstr.size();
	set_box(box, node);
	set_conf(conf, node);
	set_content(wstr, node);
}

////////////////////////////////////////////////////////////////////////////////
void 
AltoXmlParserLine::insert(size_t pos, wchar_t c)
{
	// std::cerr << "(AltoXmlParserLine) insert pos: " << pos << " " << c << "\n";
	assert(pos < chars_.size());
	chars_.insert(begin(chars_) + pos, make_copy(chars_[pos]));
	auto type = isspace(c) ? Type::Space : Type::Char; // don't care about hyphens
	chars_[pos].type = type;
	chars_[pos].c = c;
	needs_update_ = true;
}

////////////////////////////////////////////////////////////////////////////////
void 
AltoXmlParserLine::erase(size_t pos)
{
	// std::cerr << "(AltoXmlParserLine) erase pos: " << pos << "\n";
	assert(pos < chars_.size());
	chars_.erase(begin(chars_) + pos);	
	needs_update_ = true;
}

////////////////////////////////////////////////////////////////////////////////
void 
AltoXmlParserLine::set(size_t pos, wchar_t c)
{
	// std::cerr << "(AltoXmlParserLine) set pos: " << pos << " " << c << "\n";
	assert(pos < chars_.size());

	auto type = isspace(c) ? Type::Space : Type::Char; // don't care about hyphens
	chars_[pos].type = type;
	chars_[pos].c = c;
	needs_update_ = true;
}

////////////////////////////////////////////////////////////////////////////////
void
AltoXmlParserLine::init_string(const pugi::xml_node& node) 
{
	assert(strcmp(node.name(), "String") == 0);

	const auto wc = node.attribute("WC").as_double();
	const auto box = get_box(node);
	const auto token = node.attribute("CONTENT").value();
	const auto len = strlen(token);
	std::wstring wstr;
	utf8::utf8to32(token, token + len, std::back_inserter(wstr));
	const auto boxes = box.split(wstr.size());
	assert(boxes.size() == wstr.size());
	for (auto i = 0U; i < wstr.size(); ++i) {
		chars_.push_back(Char{
			boxes[i],
			node,
			Type::Char,
			wc,
			wstr[i]
		});
	}
}

////////////////////////////////////////////////////////////////////////////////
void
AltoXmlParserLine::init_space(const Node& node) 
{
	assert(strcmp(node.name(), "SP") == 0);
	auto wc = node.attribute("WC").as_double();
	chars_.push_back(Char{
		get_box(node),
		node,
		Type::Space,
		wc,
		L' '
	});
}

////////////////////////////////////////////////////////////////////////////////
void
AltoXmlParserLine::init_hyphen(const Node& node) 
{
	assert(strcmp(node.name(), "HYP") == 0);
	auto wc = node.attribute("WC").as_double();
	chars_.push_back(Char{
		get_box(node),
		node,
		Type::Hyphen,
		wc,
		L'-'
	});
}

////////////////////////////////////////////////////////////////////////////////
void
AltoXmlParserLine::init()
{
	for (const auto& node: node_.children()) {
		if (strcmp(node.name(), "String") == 0) {
			init_string(node);
		} else if (strcmp(node.name(), "SP") == 0) {
			init_space(node);
		} else if (strcmp(node.name(), "HYP")) {
			init_hyphen(node);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
Box
AltoXmlParserLine::get_box(const Node& node) 
{
	const auto x0 = node.attribute("HPOS").as_int();
	const auto y0 = node.attribute("VPOS").as_int();
	const auto w = node.attribute("WIDTH").as_int();
	const auto h = node.attribute("HEIGHT").as_int();
	return Box {x0, y0, x0 + w, y0 + h};
}	
////////////////////////////////////////////////////////////////////////////////
void 
AltoXmlParserLine::set_box(const Box& box, Node& node)
{
	if (not node.attribute("HPOS"))
		node.append_attribute("HPOS");
	if (not node.attribute("VPOS"))
		node.append_attribute("VPOS");
	if (not node.attribute("WIDTH"))
		node.append_attribute("WIDTH");
	if (not node.attribute("HEIGHT"))
		node.append_attribute("HEIGHT");
	node.attribute("HPOS").set_value(box.left());
	node.attribute("VPOS").set_value(box.top());
	node.attribute("WIDTH").set_value(box.width());
	node.attribute("HEIGHT").set_value(box.height());
}

////////////////////////////////////////////////////////////////////////////////
void 
AltoXmlParserLine::set_conf(double conf, Node& node)
{
	if (not node.attribute("WC"))
		node.append_attribute("WC");
	node.attribute("WC").set_value(conf);
}

////////////////////////////////////////////////////////////////////////////////
void 
AltoXmlParserLine::set_content(const std::wstring& str, Node& node)
{
	if (not node.attribute("CONTENT"))
		node.append_attribute("CONTENT");
	std::string ustr;
	ustr.reserve(str.size());
	utf8::utf32to8(begin(str), end(str), std::back_inserter(ustr));
	node.attribute("CONTENT").set_value(ustr.data());
}

////////////////////////////////////////////////////////////////////////////////
AltoXmlParserLine::Iterator 
AltoXmlParserLine::find_end_of_token(Iterator b, Iterator e) noexcept
{
	assert(b != e);
	switch (b->type) {
	case Type::Char:
		return std::find_if(b + 1, e, [](const auto& c) {
			return c.type != Type::Char;
		});
	default:
		return b + 1;
	}
}

////////////////////////////////////////////////////////////////////////////////
AltoXmlParserLine::Char 
AltoXmlParserLine::make_copy(Char& c)
{
	auto split = c.box.split(2);
	auto copy = c;
	copy.box = split[0];
	c.box = split[1];
	return copy;	
}
////////////////////////////////////////////////////////////////////////////////
AltoXmlParserLine::Node 
AltoXmlParserLine::merge(Node& a, const Node& b)
{
	if (a != b) { // don't merge the same node with itself
		for (const auto& attr: b.attributes()) {
			if (not a.attribute(attr.name())) 
				a.append_attribute(attr.name()).set_value(attr.value());
		}
	}
	return a;
}
