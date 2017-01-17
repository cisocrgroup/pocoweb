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
LineBuilder::LineBuilder(): line_(), page_() {reset();}

////////////////////////////////////////////////////////////////////////////////
LineBuilder&
LineBuilder::reset()
{
	line_ = std::make_shared<Line>(0);
	// do not reset page_; keep it until reset with set_page
	return *this;
}

////////////////////////////////////////////////////////////////////////////////
LineBuilder&
LineBuilder::append(const std::string& str, int r, double conf)
{
	PRAECONDITION;
	std::wstring wstr;
	wstr.reserve(str.size());
	utf8::utf8to32(begin(str), end(str), std::back_inserter(wstr));
	return append(wstr, r, conf);
}

////////////////////////////////////////////////////////////////////////////////
LineBuilder&
LineBuilder::append(const std::wstring& wstr, int r, double conf)
{
	PRAECONDITION;
	if (not wstr.empty()) {
		auto l = line_->chars_.empty() ? 0 : line_->chars_.back().cut;
		auto d = (r - l) / static_cast<int>(wstr.size());
		for (auto c: wstr) {
			l += d;
			append(c, 0, l, conf);
		}
		assert(not line_->chars_.empty());
		// fix error (not bug) in d
		line_->chars_.back().cut = r;
	}
	return *this;
}

////////////////////////////////////////////////////////////////////////////////
LineBuilder&
LineBuilder::append(wchar_t c, int r, double conf)
{
	PRAECONDITION;
	return append(c, 0, r, conf);
}

////////////////////////////////////////////////////////////////////////////////
LineBuilder&
LineBuilder::append(wchar_t o, wchar_t c, int r, double conf)
{
	PRAECONDITION;
	line_->chars_.emplace_back(o, c, r, conf);
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
	return line_;
}

#undef PREACONDITION

