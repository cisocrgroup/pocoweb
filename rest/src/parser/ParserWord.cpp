#include <algorithm>
#include <cassert>
#include <utf8.h>
#include "ParserChar.hpp"
#include "ParserWord.hpp"

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
void
ParserWord::update()
{
	std::wstring str;
	str.reserve(chars_.size());
	for (auto& c: chars_) {
		assert(c);
		str.push_back(c->get());
		box_ += c->box();
		c->set_word(shared_from_this());
	}
	std::string ustr;
	ustr.reserve(str.size());
	utf8::utf32to8(begin(str), end(str), std::back_inserter(ustr));	
	update(ustr);
}

////////////////////////////////////////////////////////////////////////////////
void 
ParserWord::remove(ParserWordChar* c)
{
	auto i = std::find(begin(chars_), end(chars_), c);
	if (i != end(chars_)) {
		chars_.erase(i);
		if (chars_.empty())
			remove();
	}
}

////////////////////////////////////////////////////////////////////////////////
void 
ParserWord::push_back(ParserWordChar* new_char)
{
	if (new_char) {
		new_char->set_word(shared_from_this());
		chars_.push_back(new_char);
	}
}

////////////////////////////////////////////////////////////////////////////////
void 
ParserWord::push_front(ParserWordChar* new_char)
{
	if (new_char) {
		new_char->set_word(shared_from_this());
		chars_.insert(begin(chars_), new_char);
	}
}

////////////////////////////////////////////////////////////////////////////////
void 
ParserWord::insert(ParserWordChar* new_char, ParserWordChar* at)
{
	if (new_char) {
		new_char->set_word(shared_from_this());
		auto i = std::find(begin(chars_), end(chars_), at);
		chars_.insert(i, new_char);
	}
}

////////////////////////////////////////////////////////////////////////////////
void 
ParserWord::merge(ParserWordChar* at, ParserWord& word)
{
	if (at)
		chars_.push_back(at);
	std::copy(begin(word.chars_), end(word.chars_), std::back_inserter(chars_));
	for (auto c: chars_) {
		assert(c);
		c->set_word(shared_from_this());
	}
	word.remove();
	update();
}

////////////////////////////////////////////////////////////////////////////////
void 
ParserWord::split(ParserWordChar* at)
{
	if (not at)
		return;
	auto b = begin(chars_);
	auto e = end(chars_);
	auto i = std::find(b, e, at);
	if (i == e)
		return;

	std::vector<ParserWordChar*> left, right;
	std::copy(b, i, std::back_inserter(left));
	std::copy(std::next(i), e, std::back_inserter(right));
	
	auto r = create();
	if (not right.empty())
		r->box_ = right.front()->box();
	std::swap(left, this->chars_);
	std::swap(right, r->chars_);
	this->update();
	r->update();
}
