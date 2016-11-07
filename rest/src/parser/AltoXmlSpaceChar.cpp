#include <cassert>
#include <cstring>
#include <utf8.h>
#include "AltoXmlParserWord.hpp"
#include "AltoXmlSpaceChar.hpp"

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
