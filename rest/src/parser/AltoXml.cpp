#include <iostream>
#include <cassert>
#include <cstring>
#include <utf8.h>
#include "ParserPage.hpp"
#include "AltoXml.hpp"

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
AltoXmlSpaceChar::AltoXmlSpaceChar(const pugi::xml_node& node)
	: ParserChar({}, L' ')
	, node_(node)
{
	if (strcmp(node_.name(), "SP") != 0) 
		throw std::logic_error(
			"(AltoXmlSpaceChar) Invalid ALTO space node: " +
			std::string(node_.name())
		);
	const auto l = node_.attribute("HPOS").as_int();
	const auto t = node_.attribute("VPOS").as_int();
	const auto w = node_.attribute("WIDTH").as_int();
	const auto h = node_.attribute("HEIGHT").as_int();
	
	box_ = {l, t, l + w, t + h};
}	

////////////////////////////////////////////////////////////////////////////////
ParserCharPtr 
AltoXmlSpaceChar::set(wchar_t c)
{
	assert(node_);
	if (isspace(c)) // skip whitespace
		return shared_from_this();

	auto box = this->box();
	auto conf = this->conf();
	auto new_char = std::make_shared<ParserWordChar>(box, c, conf);
	if (left_ and right_) {
		left_->merge(new_char.get(), *right_);
		node_.parent().remove_child(node_);
	} else if (left_) {
		left_->push_back(new_char.get());
		node_.parent().remove_child(node_);
	} else if (right_) {
		right_->push_front(new_char.get());
		node_.parent().remove_child(node_);
	} else {
		return new_token(c);
	}
	return new_char;
}

////////////////////////////////////////////////////////////////////////////////
void 
AltoXmlSpaceChar::remove()
{
	assert(node_);
	if (left_ and right_) { // merge
		left_->merge(nullptr, *right_);
	}
	auto parent = node_.parent();
	parent.remove_child(node_);
	node_ = pugi::xml_node();
}

////////////////////////////////////////////////////////////////////////////////
ParserCharPtr 
AltoXmlSpaceChar::insert(wchar_t c)
{
	auto box = this->box();
	auto conf = this->conf();
	if (isspace(c)) {
		auto copy = node_.insert_copy_before(node_, node_);
		auto res = std::make_shared<AltoXmlSpaceChar>(copy);
		res->left_ = std::move(left_);
		return res;
	} else {
		if (left_) {
			auto boxes = box.split(2);
			assert(boxes.size() == 2);
			auto res = std::make_shared<ParserWordChar>(boxes[0], c, conf);
			left_->push_back(res.get());
			return res;
		} else {
			auto res = new_token(c);
			left_ = std::dynamic_pointer_cast<AltoXmlParserWord>(res->word());
			return res;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
ParserWordCharPtr
AltoXmlSpaceChar::new_token(wchar_t c)
{
	char buf[] = {0,0,0,0,0};
	utf8::utf32to8(&c, &c + 1, buf);
	node_.set_name("String");
	node_.append_attribute("VPOS").set_value(0);
	node_.append_attribute("HPOS").set_value(0);
	node_.append_attribute("WIDTH").set_value(0);
	node_.append_attribute("HEIGHT").set_value(0);
	node_.append_attribute("CONTENT").set_value(buf);

	auto word = std::make_shared<AltoXmlParserWord>(node_);
	auto new_char = std::make_shared<ParserWordChar>(box(), c, conf());
	new_char->set_word(word);
	word->push_back(new_char.get());
	return new_char;
}

////////////////////////////////////////////////////////////////////////////////
double 
AltoXmlSpaceChar::conf() const
{
	if (left_)
		return left_->conf();
	if (right_)
		return right_->conf();
	return 0.0;
}

////////////////////////////////////////////////////////////////////////////////
AltoXmlChar::AltoXmlChar(Box box, wchar_t c, double conf, AltoXmlParserWordPtr w)
	: ParserWordChar(box, c, conf, std::move(w))
{
}

////////////////////////////////////////////////////////////////////////////////
ParserCharPtr
AltoXmlChar::set(wchar_t c)
{
	return ParserWordChar::set(c);
}

////////////////////////////////////////////////////////////////////////////////
ParserCharPtr
AltoXmlChar::insert(wchar_t c)
{
	return ParserWordChar::set(c);
}

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
	std::cerr << "(AltoXmlParser) word: " << word << "\n";
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
		auto c = std::make_shared<AltoXmlChar>(
			boxes[i],
			str[i],
			conf_,
			shared_from_this()
		);
		// line.chars.push_back(c);
		chars_.push_back(c.get());
	}
}

////////////////////////////////////////////////////////////////////////////////
ParserWordPtr 
AltoXmlParserWord::create() 
{
	auto copy = node_.insert_copy_after(node_, node_);
	return std::make_shared<AltoXmlParserWord>(copy);	
}
