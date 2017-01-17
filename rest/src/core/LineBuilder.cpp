#include <cwchar>
#include <utf8.h>
#include <cassert>
#include <cstring>
#include "Line.hpp"
#include "Box.hpp"
#include "LineBuilder.hpp"

using namespace pcw;

#define PRAECONDITION \
	assert(line_); \
	assert(r >= 0 and conf >= 0 and conf <= 1.0)

////////////////////////////////////////////////////////////////////////////////
LineBuilder::LineBuilder(): contents_(), line_(), page_() {reset();}

////////////////////////////////////////////////////////////////////////////////
LineBuilder&
LineBuilder::reset()
{
	contents_.clear();
	line_ = std::make_shared<Line>(0);
	// do not reset page_; keep it until reset with set_page
	return *this;
}

////////////////////////////////////////////////////////////////////////////////
LineBuilder&
LineBuilder::append(const std::string& str, int l, int r, double conf)
{
	PRAECONDITION;
	return append(str.data(), str.size(), l, r, conf);
}

////////////////////////////////////////////////////////////////////////////////
LineBuilder&
LineBuilder::append(const std::wstring& wstr, int l, int r, double conf)
{
	PRAECONDITION;
	return append(wstr.data(), wstr.size(), l, r, conf);
}

////////////////////////////////////////////////////////////////////////////////
LineBuilder&
LineBuilder::append(const char* str, size_t n, int l, int r, double conf)
{
	PRAECONDITION;
	if (not str or not *str)
		return *this;
	std::wstring wstr;
	utf8::utf8to32(str, str + n, std::back_inserter(wstr));
	return append(wstr.data(), wstr.size(), l, r, conf);
}

////////////////////////////////////////////////////////////////////////////////
LineBuilder&
LineBuilder::append(const char* str, int l, int r, double conf)
{
	PRAECONDITION;
	if (not str or not *str)
		return *this;
	return append(str, strlen(str), l, r, conf);
}

////////////////////////////////////////////////////////////////////////////////
LineBuilder&
LineBuilder::append(const wchar_t* str, size_t n, int l, int r, double conf)
{
	PRAECONDITION;
	if (not str or not *str)
		return *this;
	const auto d = line_->box.width() / n;
	int cut = l;
	while (*str) {
		append(*str++, cut, conf);
		cut += d;
	}
	std::get<2>(contents_.back()) = r; // fix last pos of string due to error in d
	return *this;
}

////////////////////////////////////////////////////////////////////////////////
LineBuilder&
LineBuilder::append(const wchar_t* str, int l, int r, double conf)
{
	PRAECONDITION;
	if (not str or not *str)
		return *this;
	return append(str, wcslen(str), l, r, conf);
}

////////////////////////////////////////////////////////////////////////////////
LineBuilder&
LineBuilder::append(wchar_t c, int r, double conf)
{
	PRAECONDITION;
	contents_.emplace_back(c, 0, r, conf);
	return *this;
}

////////////////////////////////////////////////////////////////////////////////
LineBuilder&
LineBuilder::append(wchar_t o, wchar_t c, int r, double conf)
{
	PRAECONDITION;
	contents_.emplace_back(o, c, r, conf);
	return *this;
}

////////////////////////////////////////////////////////////////////////////////
LineBuilder&
LineBuilder::set_image(Path image)
{
	line_->img = std::move(image);
	return *this;
}

////////////////////////////////////////////////////////////////////////////////
LineBuilder&
LineBuilder::set_box(Box box)
{
	line_->box = box;
	return *this;
}

////////////////////////////////////////////////////////////////////////////////
LineBuilder&
LineBuilder::set_page(PageSptr page)
{
	page_ = page; // hold on to the page, since line_ only holds a week ptr
	line_->page_ = std::move(page);
	return *this;
}

////////////////////////////////////////////////////////////////////////////////
LineSptr
LineBuilder::build() const
{
	assert(line_);
	for (const auto& t: contents_) {
		line_->chars_.emplace_back(
			std::get<0>(t), std::get<1>(t),
			std::get<2>(t), std::get<3>(t));
	}
	return line_;
}

#undef PREACONDITION

