#include "core/WagnerFischer.hpp"
#include "ParserChar.hpp"
#include "ParserPage.hpp"

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
std::wstring 
ParserLine::wstring() const noexcept 
{
	std::wstring res(chars.size(), 0);
	std::transform(begin(chars), end(chars), begin(res), [](const auto& c) {
		return c->get();
	});
	return res;
}

////////////////////////////////////////////////////////////////////////////////
void 
ParserLine::insert(size_t pos, wchar_t c)
{
	assert(pos < chars.size());
	auto i = chars[pos]->insert(c);
	chars.insert(begin(chars) + pos, i);
}

////////////////////////////////////////////////////////////////////////////////
void 
ParserLine::erase(size_t pos)
{
	assert(pos < chars.size());
	chars[pos]->remove();
	chars.erase(begin(chars) + pos);	
}

////////////////////////////////////////////////////////////////////////////////
void 
ParserLine::set(size_t pos, wchar_t c)
{
	assert(pos < chars.size());
	auto i = chars[pos]->set(c);
	chars[pos] = i;
}
