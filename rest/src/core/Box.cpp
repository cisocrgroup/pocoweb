#include <algorithm>
#include "Box.hpp"

using namespace pcw;

///////////////////////////////////////////////////////////////////////////////
Box::Box(int l, int t, int r, int b)
	: left_(std::max(0, l))
	, top_(std::max(0, t))
	, right_(std::max(0, r))
	, bottom_(std::max(0, b))
{
}

///////////////////////////////////////////////////////////////////////////////
Box& 
Box::increase_left(int d) noexcept
{
	left_ = std::max(0, left_ + d); // d can be smaller than 0
	return *this;
}

///////////////////////////////////////////////////////////////////////////////
Box& 
Box::increase_right(int d) noexcept
{
	right_ = std::max(0, right_ + d); // d can be smaller than 0
	return *this;
}

///////////////////////////////////////////////////////////////////////////////
Box& 
Box::increase_top(int d) noexcept
{
	top_ = std::max(0, top_ + d); // d can be smaller than 0
	return *this;
}

///////////////////////////////////////////////////////////////////////////////
Box& 
Box::increase_bottom(int d) noexcept
{
	bottom_ = std::max(0, bottom_ + d); // d can be smaller than 0
	return *this;
}

///////////////////////////////////////////////////////////////////////////////
Box& 
Box::increase(int d) noexcept
{
	return increase_left(d).increase_right(d).increase_top(d).increase_bottom(d);
}

///////////////////////////////////////////////////////////////////////////////
Box& 
Box::operator+=(const Box& other)
{
	left_ = std::min(left_, other.left_);	
	top_ = std::min(top_, other.top_);	
	right_ = std::max(right_, other.right_);	
	bottom_ = std::max(bottom_, other.bottom_);	
	return *this;
}

///////////////////////////////////////////////////////////////////////////////
std::vector<Box> 
Box::split(int n) const
{
	std::vector<Box> splits(n);
	const int width = n > 0 ? this->width() / n : 0;
	const int ratio = n > 0 ? this->width() % n : 0;

	for (int i = 0, x0 = left_; i < n; ++i) {
		int w = width;
		if (i < ratio)
			++w;
		splits[i].left_ = x0;
		splits[i].top_ = top_;
		splits[i].right_ = x0 + w;
		splits[i].bottom_ = bottom_;
		x0 += w;
	}
	return splits;
}
