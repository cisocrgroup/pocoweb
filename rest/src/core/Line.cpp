#include <cwchar>
#include <utf8.h>
#include <cassert>
#include <cstring>
#include "Book.hpp"
#include "Page.hpp"
#include "Line.hpp"

// TODO this should be part of the wagner-fisher impl
namespace pcw {
	struct EditOp {
		enum class Type {Del, Ins, Sub, Nop};
		Type type;
		wchar_t letter;
	};
}
			
using namespace pcw;

#define PRAECONDITION assert(r >= 0 and conf >= 0 and conf <= 1.0)
#define POSTCONDITION \
	assert(string_.size() == cuts_.size() and \
		string_.size() == confs_.size() and \
		string_.size() == corrs_.size())

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
Line::insert_at(size_t i, wchar_t c)
{
	assert(i < string_.size());
	i += 1;
	string_.insert(begin(string_) + i, c);
	confs_.insert(begin(confs_) + i, 1.0);
	corrs_.insert(begin(corrs_) + i, true);
	POSTCONDITION;
}

////////////////////////////////////////////////////////////////////////////////
void
Line::delete_at(size_t i)
{
	assert(i < string_.size());
	string_.erase(begin(string_) + i);
	confs_.erase(begin(confs_) + i);
	corrs_.erase(begin(corrs_) + i);
	POSTCONDITION;
}

////////////////////////////////////////////////////////////////////////////////
void 
Line::correct(const EditOps& edits, size_t offset)
{
	const auto n = edits.size();
	for (size_t i = 0; i + offset < string_.size() and i < n; ++i) {
		auto j = i + offset;
		switch (edits[j].type) {
		case EditOp::Type::Del:
			insert_at(i, edits[j].letter);
			break;	
		case EditOp::Type::Ins:
			delete_at(j);
			++offset;
			break;
		case EditOp::Type::Sub:
			string_[j] = edits[i].letter;
			corrs_[j] = true;
			break;
		case EditOp::Type::Nop:
			corrs_[j] = true;
			break;
		}
	}
	POSTCONDITION;
}

