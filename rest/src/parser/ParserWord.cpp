#include <algorithm>
#include <cassert>
#include "ParserChar.hpp"
#include "ParserWord.hpp"

using namespace pcw;

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
ParserWord::insert(ParserWordChar* new_char, ParserWordChar* at)
{
	assert(new_char);
	auto i = std::find(begin(chars_), end(chars_), at);
	chars_.insert(i, new_char);
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
