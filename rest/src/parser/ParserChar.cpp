#include "ParserWord.hpp"
#include "ParserChar.hpp"

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
ParserCharPtr 
ParserWordChar::set(wchar_t c)
{
	char_ = c;
	word_->update();
	return shared_from_this();
}

////////////////////////////////////////////////////////////////////////////////
void 
ParserWordChar::remove()
{
	word_->remove(this);
}

////////////////////////////////////////////////////////////////////////////////
ParserCharPtr 
ParserWordChar::insert(wchar_t c)
{
	auto clone = std::make_shared<ParserWordChar>(box_, c, conf_, word_);
	word_->insert(clone.get(), this);	
	return clone;
}
