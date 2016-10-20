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
