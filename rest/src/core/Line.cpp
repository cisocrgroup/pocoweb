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
bool
Line::is_corrected() const noexcept
{
	return std::all_of(begin(chars_), end(chars_), [](const auto& c) {
		return c.is_corrected();
	});
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
		res.push_back(c.get_cor());
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
		const auto cc = c.get_cor();
		utf8::utf32to8(&cc, &cc + 1, std::back_inserter(res));
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
	chars_[ii].conf = 1;
	assert(chars_[ii].is_substitution());
	assert(chars_[ii].is_corrected());
}

////////////////////////////////////////////////////////////////////////////////
void
Line::insert(size_t i, wchar_t c)
{
	const auto ii = i + ofs_;
	assert(ii <= chars_.size()); // if ii == chars_.size() append to end

	const int cut = ii < chars_.size() ? chars_[ii].cut : box.right();

	auto r = end(chars_);
	if (ii == chars_.size()) {
		chars_.emplace_back(0, c, cut, 1.0);
		r = std::prev(end(chars_));
	} else {
		r = chars_.emplace(begin(chars_) + ii, 0, c, cut, 1.0);
	}
	auto l = ii > 0 ? begin(chars_) + (ii - 1) : begin(chars_);
	divide_cuts(l, r);
	assert(chars_[ii].is_insertion());
	assert(chars_[ii].is_corrected());
}

////////////////////////////////////////////////////////////////////////////////
void
Line::divide_cuts(Chars::iterator first, Chars::iterator last)
{
	auto e = end(chars_);
	if (first != last and first != e and last != e) {
		auto left = first == begin(chars_) ? first : std::prev(first);
		const auto right = last;

		const int n = std::distance(left, right) + 1;
		const int d = right->cut - left->cut;
		const int p = d / n;

		while (++left != right) {
			left->cut = std::prev(left)->cut + p;
		}
	}
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
	chars_[ii].conf = 1;
	assert(chars_[ii].is_corrected());
}

////////////////////////////////////////////////////////////////////////////////
static Line::CharIterator
next(Line::CharIterator i, Line::CharIterator e)
{
	if (i != e) {
		// we need find_if_not in order to handle deletions (which are both
		// considered part of words and separators) correctly.
		if (i->is_word())
			return std::find_if_not(i, e, [](const auto& c){
				return c.is_word();
			});
		else
			return std::find_if_not(i, e, [](const auto& c){
				return c.is_sep();
			});
	}
	return e;
}

////////////////////////////////////////////////////////////////////////////////
void
Line::each_token(std::function<void(const Token&)> f) const
{
	Token token;
	token.box.set_top(this->box.top());
	token.box.set_bottom(this->box.bottom());
	token.line = shared_from_this();

	const auto e = end(chars_);
	const auto b = begin(chars_);
	int id = 0;

	for (auto i = b; i != e; ) {
		auto j = next(i, e);
		token.box.set_left(i != b ? std::prev(i)->cut : 0);
		token.box.set_right(j != e ? j->cut : this->box.right());
		token.begin = i;
		token.end = j;
		token.id = ++id;
		f(token);
		i = j;
	}
}

////////////////////////////////////////////////////////////////////////////////
std::vector<Token>
Line::tokens() const
{
	std::vector<Token> tokens;
	each_token([&tokens](const Token& token) {
		tokens.push_back(token);
	});
	return tokens;
}

////////////////////////////////////////////////////////////////////////////////
std::vector<Token>
Line::words() const
{
	std::vector<Token> words;
	each_token([&words](const Token& token) {
		if (token.is_normal())
			words.push_back(token);
	});
	return words;
}

////////////////////////////////////////////////////////////////////////////////
int64_t
Line::unique_id(int bid, int pid, int lid, int tid) noexcept
{
	auto r = static_cast<uint16_t>(static_cast<unsigned int>(bid));
	auto s = static_cast<uint16_t>(static_cast<unsigned int>(pid));
	auto t = static_cast<uint16_t>(static_cast<unsigned int>(lid));
	auto u = static_cast<uint16_t>(static_cast<unsigned int>(tid));

	uint64_t id = 0;
	id |= r;

	id <<= 16;
	id |= s;

	id <<= 16;
	id |= t;

	id <<= 16;
	id |= u;
	return static_cast<int64_t>(id);
}

//
// CHAR
//

////////////////////////////////////////////////////////////////////////////////
static bool
isword(wchar_t c) noexcept
{
	switch (u_charType(c)) { // all cases fall through
	case U_UPPERCASE_LETTER:
	case U_LOWERCASE_LETTER:
	case U_TITLECASE_LETTER:
	case U_MODIFIER_LETTER:
	case U_OTHER_LETTER:
	case U_DECIMAL_DIGIT_NUMBER:
	case U_LETTER_NUMBER:
	case U_OTHER_NUMBER:
	case U_NON_SPACING_MARK:
		return true;
	default:
		return false;
	}
}

////////////////////////////////////////////////////////////////////////////////
bool
Char::is_word() const noexcept
{
	auto c = get_cor();
	// c=0 means deletion and is considered to be part of a word.
	return c ? isword(c) : true;
}

////////////////////////////////////////////////////////////////////////////////
bool
Char::is_sep() const noexcept
{
	auto c = get_cor();
	// c=0 means deletion and is considered to be part of a separator.
	return c ? not isword(c) : true;
}

//
// TOKEN
//

////////////////////////////////////////////////////////////////////////////////
double
Token::average_conf() const
{
	double sum = 0, n = 0;
	Line::cor(begin, end, [&sum, &n](const Char& c) {
		sum += c.conf;
		++n;
	});
	return sum / n;
}

////////////////////////////////////////////////////////////////////////////////
std::wstring
Token::wcor_lc() const
{
	std::wstring res;
	res.reserve(std::distance(begin, end));
	Line::cor(begin, end, [&res](const Char& c) {
		res.push_back(u_tolower(c.get_cor()));
	});
	return res;
}

////////////////////////////////////////////////////////////////////////////////
std::wstring
Token::wcor() const
{
	std::wstring res;
	res.reserve(std::distance(begin, end));
	Line::cor(begin, end, [&res](const Char& c) {
		res.push_back(c.get_cor());
	});
	return res;
}

////////////////////////////////////////////////////////////////////////////////
std::wstring
Token::wocr_lc() const
{
	std::wstring res;
	res.reserve(std::distance(begin, end));
	Line::ocr(begin, end, [&res](const Char& c) {
		res.push_back(u_tolower(c.ocr));
	});
	return res;
}

////////////////////////////////////////////////////////////////////////////////
std::wstring
Token::wocr() const
{
	std::wstring res;
	res.reserve(std::distance(begin, end));
	Line::ocr(begin, end, [&res](const Char& c) {
		res.push_back(c.ocr);
	});
	return res;
}

////////////////////////////////////////////////////////////////////////////////
std::string
Token::cor_lc() const
{
	std::string res;
	res.reserve(std::distance(begin, end));
	Line::cor(begin, end, [&res](const Char& c) {
		const auto cc = u_tolower(c.get_cor());
		utf8::utf32to8(&cc, &cc + 1, std::back_inserter(res));
	});
	return res;
}

////////////////////////////////////////////////////////////////////////////////
std::string
Token::cor() const
{
	std::string res;
	res.reserve(std::distance(begin, end));
	Line::cor(begin, end, [&res](const Char& c) {
		const auto cc = c.get_cor();
		utf8::utf32to8(&cc, &cc + 1, std::back_inserter(res));
	});
	return res;
}

////////////////////////////////////////////////////////////////////////////////
std::string
Token::ocr_lc() const
{
	std::string res;
	res.reserve(std::distance(begin, end));
	Line::ocr(begin, end, [&res](const Char& c) {
		auto cc = u_tolower(c.ocr);
		utf8::utf32to8(&cc, &cc + 1, std::back_inserter(res));
	});
	return res;
}

////////////////////////////////////////////////////////////////////////////////
std::string
Token::ocr() const
{
	std::string res;
	res.reserve(std::distance(begin, end));
	Line::ocr(begin, end, [&res](const Char& c) {
		utf8::utf32to8(&c.ocr, &c.ocr + 1, std::back_inserter(res));
	});
	return res;
}

////////////////////////////////////////////////////////////////////////////////
bool
Token::is_corrected() const
{
	return std::all_of(begin, end, [](const Char& c) {
		return c.is_corrected();
	});
}

////////////////////////////////////////////////////////////////////////////////
bool
Token::is_normal() const
{
	return std::all_of(begin, end, [](const Char& c) {
		return c.is_word();
	});
}

////////////////////////////////////////////////////////////////////////////////
uint64_t
Token::unique_id() const noexcept
{
	return Line::unique_id(line->page()->book()->id(), line->page()->id(), line->id(), id);
}
