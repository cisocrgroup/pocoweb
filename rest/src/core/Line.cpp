#include <cwchar>
#include <utf8.h>
#include <cassert>
#include <cstring>
#include "Book.hpp"
#include "Page.hpp"
#include "Line.hpp"

using namespace pcw;

#define PRAECONDITION assert(r >= 0 and conf >= 0 and conf <= 1.0)
#define POSTCONDITION \
	assert(string_.size() == cuts_.size() and string_.size() == confs_.size())

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
Line::append(char c, int r, double conf)
{
	PRAECONDITION;
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

	const auto n = wcslen(str);
	const auto d = box.width() / n;
	int cut = l;
	while (*str) {
		append(*str++, cut, conf);
		cut += d;
	}
	cuts_.back() = r; // fix last pos of string due to error in d
	POSTCONDITION;
}

////////////////////////////////////////////////////////////////////////////////
void 
Line::append(wchar_t c, int r, double conf)
{
	PRAECONDITION;
	unsigned char buf[] = {0, 0, 0, 0, 0, 0, 0, 0, 0};	
	utf8::utf32to8(&c, &c + 1, buf);
	for (auto i = buf; *i; ++i) {
		string_.push_back(*i);
		cuts_.push_back(r);
		confs_.push_back(conf);
	}
	POSTCONDITION;
}
