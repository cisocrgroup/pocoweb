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
#define POSTCONDITION \
	assert(string_.size() == cuts_.size() and \
		string_.size() == confs_.size() and \
		string_.size() == corrs_.size())

////////////////////////////////////////////////////////////////////////////////
Line::Line(int i, Box b)
	: box(b)
	, img()
	, ocr()
	, string_()
	, cuts_()
	, confs_()
	, page_()
	, id_(i)
{
}

////////////////////////////////////////////////////////////////////////////////
void
Line::append(const std::string& str, int l, int r, double conf, bool corr)
{
	append(str.data(), str.size(), l, r, conf, corr);
	POSTCONDITION;
}

////////////////////////////////////////////////////////////////////////////////
void
Line::append(const std::wstring& wstr, int l, int r, double conf, bool corr)
{
	append(wstr.data(), wstr.size(), l, r, conf, corr);
	POSTCONDITION;
}

////////////////////////////////////////////////////////////////////////////////
void
Line::append(const char* str, size_t n, int l, int r, double conf, bool corr)
{
	PRAECONDITION;
	if (not str or not *str)
		return;
	std::wstring wstr;
	utf8::utf8to32(str, str + n, std::back_inserter(wstr));
	append(wstr.data(), wstr.size(), l, r, conf, corr);
	POSTCONDITION;
}

////////////////////////////////////////////////////////////////////////////////
void
Line::append(const char* str, int l, int r, double conf, bool corr)
{
	PRAECONDITION;
	if (not str or not *str)
		return;
	append(str, strlen(str), l, r, conf, corr);
	POSTCONDITION;
}

////////////////////////////////////////////////////////////////////////////////
void 
Line::append(const wchar_t* str, size_t n, int l, int r, double conf, bool corr)
{
	PRAECONDITION;
	if (not str or not *str)
		return;
	const auto d = box.width() / n;
	int cut = l;
	while (*str) {
		append(*str++, cut, conf, corr);
		cut += d;
	}
	cuts_.back() = r; // fix last pos of string due to error in d
	POSTCONDITION;
}

////////////////////////////////////////////////////////////////////////////////
void 
Line::append(const wchar_t* str, int l, int r, double conf, bool corr)
{
	PRAECONDITION;
	if (not str or not *str)
		return;
	append(str, wcslen(str), l, r, conf, corr);
	POSTCONDITION;
}

////////////////////////////////////////////////////////////////////////////////
void 
Line::append(wchar_t c, int r, double conf, bool corr)
{
	PRAECONDITION;
	string_.push_back(c);
	cuts_.push_back(r);
	confs_.push_back(conf);
	corrs_.push_back(corr);
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

////////////////////////////////////////////////////////////////////////////////
void 
Line::set(size_t i, wchar_t c)
{
	assert(i < string_.size());
	string_[i] = c;
	corrs_[i] = true;
}

////////////////////////////////////////////////////////////////////////////////
void 
Line::insert(size_t i, wchar_t c)
{
	assert(i < string_.size());
	string_.insert(i, 1, c);
	corrs_.insert(begin(corrs_) + i, true);
	confs_.insert(begin(confs_) + i, 1.0);
	const int l = i > 0 ? cuts_[i - 1] : 0;
	const int r = cuts_[i];
	const int half = (r - l) / 2; // fuck the consequences!
	cuts_.insert(begin(cuts_) + i, l + half);
	POSTCONDITION;
}

////////////////////////////////////////////////////////////////////////////////
void 
Line::erase(size_t i)
{
	assert( i < string_.size());
	string_.erase(begin(string_) + i);
	corrs_.erase(begin(corrs_) + i);
	confs_.erase(begin(confs_) + i);
	cuts_.erase(begin(cuts_) + i);
	POSTCONDITION;
}

////////////////////////////////////////////////////////////////////////////////
void
Line::correct(const WagnerFischer& wf)
{
	int o = 0;
	const auto n = wf.trace().size();
	for (size_t i = 0; i < n; ++i) {
		const auto ii = i + o;
		switch (wf.trace()[i]) {
		// insertion means that the ocr recognized a character
		// where in reality there should be none; delete it
		case WagnerFischer::EditOp::Ins:
			erase(ii);
			--o;	
			break;
		// deletion means that the ocr did not recognize a character 
		// where in reality there should be one; so insert it
		case WagnerFischer::EditOp::Del: 
			insert(ii, wf.truth()[i]);
			break;
		case WagnerFischer::EditOp::Sub:
			set(ii, wf.truth()[i]);
			break;
		case WagnerFischer::EditOp::Nop:
			break;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
static bool
isword(wchar_t c)
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
void 
Line::each_word(std::function<void(const Word&)> f) const
{
	Word word;
	word.box.set_top(this->box.top());
	word.box.set_bottom(this->box.bottom());
	word.word.reserve(string_.size() * 2);

	auto e = end(string_);
	auto b = begin(string_);
	auto i = std::find_if(begin(string_), e, isword);
	while (i != e) {
		auto j = std::find_if_not(i, e, isword);
		auto pi = std::distance(b, i);
		auto pj = std::distance(b, j);
		double n = std::distance(i, j);

		auto corr = std::all_of(begin(corrs_) + pi, begin(corrs_) + pj, [](bool b) {return b;});
		auto conf = std::accumulate(begin(confs_) + pi, begin(confs_) + pj, 0.0);
		word.box.set_left(pi > 0 ? cuts_[pi - 1] : 0);
		word.box.set_right(pj > 0 ? cuts_[pj - 1] : cuts_[pj]);
		word.confidence = conf / n;
		word.corrected = corr;
		utf8::utf32to8(i, j, std::back_inserter(word.word));
		f(word);
		word.word.clear();
		i = std::find_if(j, e, isword);
	}
}
