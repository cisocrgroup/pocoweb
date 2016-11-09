#include <iostream>
#include <fstream>
#include <utf8.h>
#include "core/WagnerFischer.hpp"
#include "ParserChar.hpp"
#include "ParserPage.hpp"

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
void 
ParserPage::write(const Path& path) const
{
	std::ofstream os(path.string());
	os << "\n";
	os.close();
}

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
std::string 
ParserLine::string() const noexcept 
{
	std::string res;
	res.reserve(chars.size());
	std::for_each(begin(chars), end(chars), [&res](const auto& c) {
		const auto wc = c->get();
		utf8::utf32to8(&wc, &wc + 1, std::back_inserter(res));
	});
	return res;
}

////////////////////////////////////////////////////////////////////////////////
void 
ParserLine::insert(size_t pos, wchar_t c)
{
	std::cerr << "insert pos: " << pos << " " << c << "\n";
	assert(pos < chars.size());
	auto i = chars[pos]->insert(c);
	chars.insert(begin(chars) + pos, i);
}

////////////////////////////////////////////////////////////////////////////////
void 
ParserLine::erase(size_t pos)
{
	std::cerr << "erase pos: " << pos << "\n";
	assert(pos < chars.size());
	chars[pos]->remove();
	chars.erase(begin(chars) + pos);	
}

////////////////////////////////////////////////////////////////////////////////
void 
ParserLine::set(size_t pos, wchar_t c)
{
	std::cerr << "set pos: " << pos << " " << c << "\n";
	assert(pos < chars.size());
	auto i = chars[pos]->set(c);
	chars[pos] = i;
}
