#include <unicode/uchar.h>
#include <cwchar>
#include <utf8.h>
#include <cassert>
#include <cstring>
#include "WagnerFischer.hpp"
#include "Book.hpp"
#include "Page.hpp"
#include "Line.hpp"

using namespace pcw;

#define PRAECONDITION assert(r >= 0 and conf >= 0 and conf <= 1.0)

////////////////////////////////////////////////////////////////////////////////
Line::Line(int i, Box b)
	: box(b)
	, img()
	, chars_()
	, page_()
	, id_(i)
	, ofs_()
{
}

////////////////////////////////////////////////////////////////////////////////
void
Line::append(const std::string& str, int l, int r, double conf)
{
	PRAECONDITION;
	append(str.data(), str.size(), l, r, conf);
}

////////////////////////////////////////////////////////////////////////////////
void
Line::append(const std::wstring& wstr, int l, int r, double conf)
{
	PRAECONDITION;
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
}

////////////////////////////////////////////////////////////////////////////////
void
Line::append(const char* str, int l, int r, double conf)
{
	PRAECONDITION;
	if (not str or not *str)
		return;
	append(str, strlen(str), l, r, conf);
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
	chars_.back().cut = r; // fix last pos of string due to error in d
}

////////////////////////////////////////////////////////////////////////////////
void 
Line::append(const wchar_t* str, int l, int r, double conf)
{
	PRAECONDITION;
	if (not str or not *str)
		return;
	append(str, wcslen(str), l, r, conf);
}

////////////////////////////////////////////////////////////////////////////////
void 
Line::append(wchar_t c, int r, double conf)
{
	PRAECONDITION;
	chars_.emplace_back(c, 0, r, conf);
}

////////////////////////////////////////////////////////////////////////////////
void 
Line::append(wchar_t o, wchar_t c, int r, double conf)
{
	PRAECONDITION;
	chars_.emplace_back(o, c, r, conf);
}

////////////////////////////////////////////////////////////////////////////////
std::wstring 
Line::wcor() const
{
	std::wstring res;
	res.reserve(chars_.size());
	cor(begin(chars_), end(chars_), [&res](const Char& c) {
		res.push_back(c.cor);
	});
	return res;
}

////////////////////////////////////////////////////////////////////////////////
std::wstring 
Line::wocr() const
{
	std::wstring res;
	res.reserve(chars_.size());
	ocr(begin(chars_), end(chars_), [&res](const Char& c) {
		if (c.cor)
			res.push_back(c.cor);
		else if (c.ocr)
			res.push_back(c.ocr);
	});
	return res;
}

////////////////////////////////////////////////////////////////////////////////
std::string 
Line::cor() const
{
	std::string res;
	res.reserve(chars_.size());
	cor(begin(chars_), end(chars_), [&res](const Char& c) {
		if (c.cor)
			utf8::utf32to8(&c.cor, &c.cor + 1, std::back_inserter(res));
		else if (c.ocr)
			utf8::utf32to8(&c.ocr, &c.ocr + 1, std::back_inserter(res));
	});
	return res;
}

////////////////////////////////////////////////////////////////////////////////
std::string 
Line::ocr() const
{
	std::string res;
	res.reserve(chars_.size());
	ocr(begin(chars_), end(chars_), [&res](const Char& c) {
		utf8::utf32to8(&c.ocr, &c.ocr + 1, std::back_inserter(res));
	});
	return res;
}

////////////////////////////////////////////////////////////////////////////////
Line::Confidences 
Line::confidences() const 
{
	Confidences confidences;
	confidences.reserve(chars_.size());
	cor(begin(chars_), end(chars_), [&confidences](const Char& c){
		confidences.push_back(c.conf);
	});
	return confidences;
}

////////////////////////////////////////////////////////////////////////////////
Line::Cuts 
Line::cuts() const 
{
	Cuts cuts;
	cuts.reserve(chars_.size());
	cor(begin(chars_), end(chars_), [&cuts](const Char& c){
		cuts.push_back(c.cut);
	});
	return cuts;
}

////////////////////////////////////////////////////////////////////////////////
double 
Line::average_conf() const noexcept
{
	double sum = 0;
	double n = 0;
	cor(begin(chars_), end(chars_), [&sum,&n](const Char& c) {
		++n; 
		sum += c.conf;
	});
	return sum / n;
}

////////////////////////////////////////////////////////////////////////////////
void
Line::begin_wagner_fischer() noexcept
{
	ofs_ = 0;
}

////////////////////////////////////////////////////////////////////////////////
void 
Line::set(size_t i, wchar_t c)
{
	const auto ii = i + ofs_;
	assert(ii < chars_.size());
	chars_[ii].cor = c;
	assert(chars_[ii].is_substitution());
	assert(chars_[ii].is_corrected());
}

////////////////////////////////////////////////////////////////////////////////
void 
Line::insert(size_t i, wchar_t c)
{
	const auto ii = i + ofs_;
	assert(ii < chars_.size());
	const int l = ii > 0 ? chars_[ii].cut : 0;
	const int r = chars_[ii].cut;
	const int half = (r - l) / 2; // fuck the consequences!
	chars_.insert(begin(chars_) + ii, Char{0, c, l + half, 1.0});
	assert(chars_[ii].is_insertion());
	assert(chars_[ii].is_corrected());
}

////////////////////////////////////////////////////////////////////////////////
void 
Line::erase(size_t i)
{
	const auto ii = i + ofs_;
	assert(ii < chars_.size());
	chars_[ii].cor = Char::DELETION;
	assert(chars_[ii].is_deletion());
	assert(chars_[ii].is_corrected());
	++ofs_;
}

////////////////////////////////////////////////////////////////////////////////
void 
Line::noop(size_t i)
{
	const auto ii = i + ofs_;
	assert(ii < chars_.size());
	chars_[ii].cor = chars_[ii].ocr;
	assert(chars_[ii].is_corrected());
}

////////////////////////////////////////////////////////////////////////////////
static bool
isword(wchar_t c) noexcept
{
	switch (u_charType(c)) {     // all cases fall through
	case U_UPPERCASE_LETTER:     // Lu
	case U_LOWERCASE_LETTER:     // Al
	case U_TITLECASE_LETTER:     // Lt
	case U_MODIFIER_LETTER:      // Lm
	case U_OTHER_LETTER:         // Lo
	case U_DECIMAL_DIGIT_NUMBER: // Nd
	case U_LETTER_NUMBER:        // Nl
	case U_OTHER_NUMBER:         // No
	case U_NON_SPACING_MARK:     // Mn
		return true;
	default:
		return false;
	}
}

////////////////////////////////////////////////////////////////////////////////
static bool
cisword(const Line::Char& c) noexcept
{
	return isword(c.get_cor());
}

////////////////////////////////////////////////////////////////////////////////
void 
Line::each_token(std::function<void(const Token&)> f) const
{
	Token token;
	token.box.set_top(this->box.top());
	token.box.set_bottom(this->box.bottom());

	const auto e = end(chars_);
	const auto b = begin(chars_);
	auto i = std::find_if(b, e, cisword);
	while (i != e) {
		auto j = std::find_if_not(i, e, cisword);
		token.box.set_left(i != b ? std::prev(i)->cut : 0);
		token.box.set_right(j != e ? j->cut : this->box.right());
		token.range.first = i; 
		token.range.second = j;
		f(token);
		i = std::find_if(j, e, cisword);
	}
}

//
// TOKEN
//

////////////////////////////////////////////////////////////////////////////////
double 
Line::Token::average_conf() const
{
	double sum = 0, n = 0;
	Line::cor(range.first, range.second, [&sum, &n](const Char& c) {
		sum += c.conf;
		++n;
	});
	return sum / n;
}

////////////////////////////////////////////////////////////////////////////////
std::wstring 
Line::Token::wcor() const
{
	std::wstring res;
	res.reserve(std::distance(range.first, range.second));
	Line::cor(range.first, range.second, [&res](const Char& c) {
		res.push_back(c.cor);
	});
	return res;
}

////////////////////////////////////////////////////////////////////////////////
std::wstring 
Line::Token::wocr() const
{
	std::wstring res;
	res.reserve(std::distance(range.first, range.second));
	Line::ocr(range.first, range.second, [&res](const Char& c) {
		res.push_back(c.cor);
	});
	return res;
}

////////////////////////////////////////////////////////////////////////////////
std::string 
Line::Token::cor() const
{
	std::string res;
	res.reserve(std::distance(range.first, range.second));
	Line::cor(range.first, range.second, [&res](const Char& c) {
		utf8::utf32to8(&c.cor, &c.cor + 1, std::back_inserter(res));
	});
	return res;
}

////////////////////////////////////////////////////////////////////////////////
std::string 
Line::Token::ocr() const
{
	std::string res;
	res.reserve(std::distance(range.first, range.second));
	Line::ocr(range.first, range.second, [&res](const Char& c) {
		utf8::utf32to8(&c.ocr, &c.ocr + 1, std::back_inserter(res));
	});
	return res;
}

////////////////////////////////////////////////////////////////////////////////
bool 
Line::Token::is_corrected() const
{
	return std::all_of(range.first, range.second, [](const Char& c) {
		return c.is_corrected();
	});
}

////////////////////////////////////////////////////////////////////////////////
bool 
Line::Token::is_normal() const
{
	return std::all_of(range.first, range.second, [](const Char& c) {
		return isword(c.cor);
	});
}

////////////////////////////////////////////////////////////////////////////////
bool 
Line::Token::is_space() const
{
	return std::all_of(range.first, range.second, [](const Char& c) {
		return isspace(c.cor);
	});
}
