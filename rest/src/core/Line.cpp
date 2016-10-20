#include <cwchar>
#include <utf8.h>
#include <cassert>
#include <cstring>
#include "Book.hpp"
#include "Page.hpp"
#include "Line.hpp"

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
Line::Line(Page& page, int i, Box b)
	: page(page.shared_from_this())
	, box(b)
	, id(i)
	, string_()
	, cuts_()
{
}

////////////////////////////////////////////////////////////////////////////////
void
Line::append(const char* str, const Box& box)
{
	if (not str or not *str)
		return;

	std::wstring wstr;
	const int n = strlen(str);
	utf8::utf8to32(str, str + n, std::back_inserter(wstr));
	append(wstr.data(), box);
	assert(string_.size() == cuts_.size() + 1);
}

////////////////////////////////////////////////////////////////////////////////
void
Line::append(char c, const Box& box)
{
	if (empty())
		cuts_.push_back(box.left());
	string_.push_back(c);
	cuts_.push_back(box.right());
	assert(string_.size() == cuts_.size() + 1);
}

////////////////////////////////////////////////////////////////////////////////
void 
Line::append(const wchar_t* str, const Box& box)
{
	if (not str or not *str)
		return;
	const auto n = wcslen(str);
	const auto d = box.width() / n;
	while (*str) {
		const auto l = cuts_.back();
		const auto r = cuts_.back() + d;
		append(*str++, l, r);
	}
	cuts_.back() = box.right(); // fix last pos of string due to error in d
	assert(string_.size() == cuts_.size() + 1);
}

////////////////////////////////////////////////////////////////////////////////
void 
Line::append(wchar_t c, int l, int r)
{
	if (empty())
		cuts_.push_back(l);
	unsigned char buf[] = {0, 0, 0, 0, 0, 0, 0, 0, 0};	
	utf8::utf32to8(&c, &c + 1, buf);
	for (auto i = buf; *i; ++i) {
		string_.push_back(*i);
		cuts_.push_back(r);
	}
	assert(string_.size() == cuts_.size() + 1);
}
