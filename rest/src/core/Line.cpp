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

	if (empty())
		cuts_.push_back(box.left());
	const int n = strlen(str);
	const int d = box.width() / n;

	while (*str) {
		string_.push_back(*str++);
		cuts_.push_back(cuts_.back() + d);
	}
	cuts_.back() = box.right(); // fix last pos of string due to error in d
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
