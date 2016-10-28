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
	append(str.data(), str.size(), l, r, conf);
}

////////////////////////////////////////////////////////////////////////////////
void
Line::append(const std::wstring& wstr, int l, int r, double conf)
{
	append(wstr.data(), wstr.size(), l, r, conf);
}

////////////////////////////////////////////////////////////////////////////////
void
Line::append(const char* str, size_t n, int l, int r, double conf)
{
	PRAECONDITION;
	if (not str or not *str)
		return;
	std::wstring wstr;
	utf8::utf8to32(str, str + n, std::back_inserter(wstr));
	append(wstr.data(), wstr.size(), l, r, conf);
	POSTCONDITION;
}

////////////////////////////////////////////////////////////////////////////////
void
Line::append(const char* str, int l, int r, double conf)
{
	PRAECONDITION;
	if (not str or not *str)
		return;
	append(str, strlen(str), l, r, conf);
	POSTCONDITION;
}

////////////////////////////////////////////////////////////////////////////////
void 
Line::append(const wchar_t* str, size_t n, int l, int r, double conf)
{
	PRAECONDITION;
	if (not str or not *str)
		return;
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
Line::append(const wchar_t* str, int l, int r, double conf)
{
	PRAECONDITION;
	if (not str or not *str)
		return;
	append(str, wcslen(str), l, r, conf);
	POSTCONDITION;
}

////////////////////////////////////////////////////////////////////////////////
void 
Line::append(wchar_t c, int r, double conf)
{
	PRAECONDITION;
	string_.push_back(c);
	cuts_.push_back(r);
	confs_.push_back(conf);
	POSTCONDITION;
}

////////////////////////////////////////////////////////////////////////////////
std::string 
Line::string() const
{
	std::string res;	
	res.reserve(string_.size());
	utf8::utf32to8(begin(string_), end(string_), std::back_inserter(res));
	return res;
}

////////////////////////////////////////////////////////////////////////////////
double 
Line::calculate_average_confidence() const noexcept
{
	auto sum = std::accumulate(begin(confs_), end(confs_), 0.0);
	return sum / static_cast<double>(confs_.size());
}
