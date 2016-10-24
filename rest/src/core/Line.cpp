#include <cwchar>
#include <utf8.h>
#include <cassert>
#include <cstring>
#include "Book.hpp"
#include "Page.hpp"
#include "Line.hpp"

using namespace pcw;

#define PRAECONDITION assert(l >= 0 and r >= 0 and l <= r and conf >= 0 and conf <= 1.0)
#define POSTCONDITION \
	assert(string_.size() + 1 == cuts_.size() and \
	       string_.size() == confs_.size())

////////////////////////////////////////////////////////////////////////////////
Line::Line(int i, Box b)
	: box(b)
	, id(i)
	, img()
	, ocr()
	, string_()
	, cuts_()
	, confs_()
	, page_()
{
}

////////////////////////////////////////////////////////////////////////////////
void
Line::append(const std::string& str, int l, int r, double conf)
{
	append(str.data(), l, r, conf);
}

////////////////////////////////////////////////////////////////////////////////
void
Line::append(const std::wstring& wstr, int l, int r, double conf)
{
	append(wstr.data(), l, r, conf);
}

////////////////////////////////////////////////////////////////////////////////
void
Line::append(const char* str, int l, int r, double conf)
{
	PRAECONDITION;
	if (not str or not *str)
		return;
	std::wstring wstr;
	const int n = strlen(str);
	utf8::utf8to32(str, str + n, std::back_inserter(wstr));
	append(wstr.data(), l, r, conf);
	POSTCONDITION;
}

////////////////////////////////////////////////////////////////////////////////
void
Line::append(char c, int l, int r, double conf)
{
	PRAECONDITION;
	if (cuts_.empty())
		cuts_.push_back(l);
	string_.push_back(c);
	cuts_.push_back(r);
	confs_.push_back(conf);
	POSTCONDITION;
}

////////////////////////////////////////////////////////////////////////////////
void 
Line::append(const wchar_t* str, int l, int r, double conf)
{
	PRAECONDITION;
	if (not str or not *str)
		return;

	if (cuts_.empty())
		cuts_.push_back(box.left());
	const auto n = wcslen(str);
	const auto d = box.width() / n;
	while (*str) {
		assert(not cuts_.empty());
		const auto ll = cuts_.back();
		const auto rr = cuts_.back() + d;
		append(*str++, ll, rr, conf);
	}
	cuts_.back() = r; // fix last pos of string due to error in d
	POSTCONDITION;
}

////////////////////////////////////////////////////////////////////////////////
void 
Line::append(wchar_t c, int l, int r, double conf)
{
	PRAECONDITION;
	if (cuts_.empty())
		cuts_.push_back(l);
	unsigned char buf[] = {0, 0, 0, 0, 0, 0, 0, 0, 0};	
	utf8::utf32to8(&c, &c + 1, buf);
	for (auto i = buf; *i; ++i) {
		string_.push_back(*i);
		cuts_.push_back(r);
		confs_.push_back(conf);
	}
	POSTCONDITION;
}
