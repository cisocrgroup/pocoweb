#include <iostream>
#include <regex>
#include <utf8.h>
#include "core/Line.hpp"
#include "hocr.hpp"
#include "HocrParserLine.hpp"

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
HocrParserLine::HocrParserLine(pugi::xml_node node)
	: chars_()
	, node_(node) 
	, needs_update_(false)
{
	init();
	box = hocr::get_box(node);
}

////////////////////////////////////////////////////////////////////////////////
Line
HocrParserLine::line(int id) const
{
	Line line(id, box);
	for (const auto& c: chars_) {
		line.append(c.c, c.box.right(), c.conf);
	}
	return line;
}

////////////////////////////////////////////////////////////////////////////////
std::wstring 
HocrParserLine::wstring() const 
{
	std::wstring res(chars_.size(), 0);
	std::transform(begin(chars_), end(chars_), begin(res), [](const auto& c) {
		return c.c;
	});
	return res;
}

////////////////////////////////////////////////////////////////////////////////
std::string 
HocrParserLine::string() const 
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
HocrParserLine::end_wagner_fischer()
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
		if (not i->is_space) {
			update_char(i, eot, node_);
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
HocrParserLine::update_char(Iterator b, Iterator e, Node& parent)
{
	assert(b != e);
	Node node;
	if (not b->node) {
		node = parent.append_child("span");
	} else {
		node = parent.append_copy(b->node);
		node.set_name("span");
	}
	std::wstring wstr;
	double conf = 0;
	auto box = b->box;
	std::for_each(b, e, [&node,&wstr,&conf,&box](Char& c) {
		assert(not c.is_space);
		wstr.push_back(c.c);
		box += c.box;
		conf += c.conf;
		c.node = merge(node, c.node);
		
	});
	conf /= wstr.size();
	hocr::set_box(box, node);
	hocr::set_conf(conf, node);
	hocr::set_cont(wstr, node);
}

////////////////////////////////////////////////////////////////////////////////
void 
HocrParserLine::insert(size_t pos, wchar_t c)
{
	// std::cerr << "(HocrParserLine) insert pos: " << pos << " " << c << "\n";
	assert(pos < chars_.size());
	chars_.insert(begin(chars_) + pos, make_copy(chars_[pos], c));
	needs_update_ = true;
}

////////////////////////////////////////////////////////////////////////////////
void 
HocrParserLine::erase(size_t pos)
{
	// std::cerr << "(HocrParserLine) erase pos: " << pos << "\n";
	assert(pos < chars_.size());
	chars_.erase(begin(chars_) + pos);	
	needs_update_ = true;
}

////////////////////////////////////////////////////////////////////////////////
void 
HocrParserLine::set(size_t pos, wchar_t c)
{
	// std::cerr << "(HocrParserLine) set pos: " << pos << " " << c << "\n";
	assert(pos < chars_.size());
	chars_[pos].c = c;
	chars_[pos].is_space = isspace(c);
	needs_update_ = true;
}

////////////////////////////////////////////////////////////////////////////////
void
HocrParserLine::init_string(const pugi::xml_node& node, bool space_before)
{
	assert(strcmp(node.name(), "String") == 0);

	const auto wc = hocr::get_conf(node);
	const auto box = hocr::get_box(node);
	const auto token = hocr::get_cont(node);

	const auto boxes = box.split(token.size());
	assert(boxes.size() == token.size());
	if (space_before and not boxes.empty() and not chars_.empty()) {
		chars_.back().box.set_right(boxes.front().left());
	}
	for (auto i = 0U; i < token.size(); ++i) {
		chars_.emplace_back(token[i], node, wc, boxes[i]);
	}
}

////////////////////////////////////////////////////////////////////////////////
void
HocrParserLine::init_space(const Node& node) 
{
	auto token_box = hocr::get_box(node);
	Box box{
		token_box.right(),
		token_box.top(), 
		token_box.right() + 1, 
		token_box.bottom()
	};
	chars_.emplace_back(L' ', node, hocr::get_conf(node), box);
}

////////////////////////////////////////////////////////////////////////////////
void
HocrParserLine::init()
{
	bool span = false;
	for (const auto& node: node_.children()) {
		if (strcmp(node.name(), "span") == 0) {
			if (strcmp(node.attribute("class").value(), "ocrx_word") == 0) {
				if (span) {
					init_space(node);
				}
				init_string(node, span);
				span = true;
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
HocrParserLine::Iterator 
HocrParserLine::find_end_of_token(Iterator b, Iterator e) noexcept
{
	assert(b != e);
	if (b->is_space)
		return b + 1;
	else
		return std::find_if(b + 1, e, [](const auto& c) {return c.is_space;});
}

////////////////////////////////////////////////////////////////////////////////
HocrParserLine::Char 
HocrParserLine::make_copy(Char& c, wchar_t cc)
{
	auto split = c.box.split(2);
	auto copy = Char(cc, c.node, c.conf, split[0]);
	c.box = split[1];
	return copy;	
}

////////////////////////////////////////////////////////////////////////////////
HocrParserLine::Node 
HocrParserLine::merge(Node& a, const Node& b)
{
	if (a != b) { // don't merge the same node with itself
		for (const auto& attr: b.attributes()) {
			if (not a.attribute(attr.name())) 
				a.append_attribute(attr.name()).set_value(attr.value());
		}
	}
	return a;
}
