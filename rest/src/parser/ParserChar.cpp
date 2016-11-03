#include "ParserWord.hpp"
#include "ParserChar.hpp"

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
void 
ParserWordChar::set(wchar_t c)
{
	char_ = c;
	word_->update();
}

////////////////////////////////////////////////////////////////////////////////
void 
ParserWordChar::remove()
{
	word_->remove(this);
}

////////////////////////////////////////////////////////////////////////////////
ParserCharPtr 
ParserWordChar::clone()
{
	auto clone = std::make_shared<ParserWordChar>(box_, char_, conf_, word_);
	word_->insert(clone.get(), this);	
	return clone;
}
