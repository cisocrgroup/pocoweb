#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE BoxTest

#include <boost/test/unit_test.hpp>
#include <iostream>
#include "core/Box.hpp"

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(Left) {
	Box box{1, 2, 2, 2};
	BOOST_CHECK_EQUAL(box.left(), 1);
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(Top) {
	Box box{0, 1, 2, 2};
	BOOST_CHECK_EQUAL(box.top(), 1);
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(Right) {
	Box box{0, 0, 1};
	BOOST_CHECK_EQUAL(box.right(), 1);
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(Bottom) {
	Box box{0, 0, 0, 1};
	BOOST_CHECK_EQUAL(box.bottom(), 1);
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(Width) {
	Box box{1, 0, 2, 0};
	BOOST_CHECK_EQUAL(box.width(), 1);
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(Height) {
	Box box{0, 1, 0, 2};
	BOOST_CHECK_EQUAL(box.height(), 1);
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(Invalid) {
	Box box{-1, -2, -3, -4};
	BOOST_CHECK_EQUAL(box.width(), 0);
	BOOST_CHECK_EQUAL(box.height(), 0);
	BOOST_CHECK_EQUAL(box.left(), 0);
	BOOST_CHECK_EQUAL(box.top(), 0);
	BOOST_CHECK_EQUAL(box.right(), 0);
	BOOST_CHECK_EQUAL(box.bottom(), 0);
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(IncreaseLeft) {
	Box box{2, 2, 3, 3};
	box.increase_left(1);
	BOOST_CHECK_EQUAL(box, Box(1, 2, 3, 3));
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(IncreaseLeftInverted) {
	Box box{2, 2, 3, 3};
	box.increase_left(-1);
	BOOST_CHECK_EQUAL(box, Box(3, 2, 3, 3));
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(IncreaseTop) {
	Box box{2, 2, 3, 3};
	box.increase_top(1);
	BOOST_CHECK_EQUAL(box, Box(2, 1, 3, 3));
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(IncreaseTopInverted) {
	Box box{2, 2, 3, 3};
	box.increase_top(-1);
	BOOST_CHECK_EQUAL(box, Box(2, 3, 3, 3));
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(IncreaseRight) {
	Box box{2, 2, 3, 3};
	box.increase_right(1);
	BOOST_CHECK_EQUAL(box, Box(2, 2, 4, 3));
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(IncreaseRightInverted) {
	Box box{2, 2, 3, 3};
	box.increase_right(-1);
	BOOST_CHECK_EQUAL(box, Box(2, 2, 2, 3));
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(IncreaseBottom) {
	Box box{2, 2, 3, 3};
	box.increase_bottom(1);
	BOOST_CHECK_EQUAL(box, Box(2, 2, 3, 4));
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(IncreaseBottomInverted) {
	Box box{2, 2, 3, 3};
	box.increase_bottom(-1);
	BOOST_CHECK_EQUAL(box, Box(2, 2, 3, 2));
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(IncreaseAll) {
	Box box{2, 2, 3, 3};
	box.increase(1);
	BOOST_CHECK_EQUAL(box, Box(1, 1, 4, 4));
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(IncreaseAllInverted) {
	Box box{1, 1, 4, 4};
	box.increase(-1);
	BOOST_CHECK_EQUAL(box, Box(2, 2, 3, 3));
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(Split) {
	Box box{0, 0, 4, 4};
	auto splits = box.split(4);
	BOOST_CHECK_EQUAL(4, box.width());
	BOOST_CHECK_EQUAL(4, box.height());
	BOOST_CHECK_EQUAL(4, splits.size());
	for (const auto& b : splits) {
		BOOST_CHECK_EQUAL(b.height(), 4);
		BOOST_CHECK_EQUAL(b.width(), 1);
	}
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(AdditionFromUninitialized) {
	Box box;
	box += Box{1, 2, 3, 4};
	BOOST_CHECK_EQUAL(box, Box(0, 0, 3, 4));
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(AdditionBigger) {
	Box box{2, 2, 3, 3};
	box += Box{1, 1, 4, 4};
	BOOST_CHECK_EQUAL(box, Box(1, 1, 4, 4));
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(AdditionSmaller) {
	Box box{1, 1, 4, 4};
	box += Box{2, 2, 3, 3};
	BOOST_CHECK_EQUAL(box, Box(1, 1, 4, 4));
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(AdditionMixed) {
	Box box{1, 1, 3, 3};
	box += Box{2, 2, 4, 4};
	BOOST_CHECK_EQUAL(box, Box(1, 1, 4, 4));
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(IncrementalAddition) {
	auto box = Box{10, 10, 12, 12} + Box{8, 8, 13, 15} + Box{10, 7, 15, 13};
	BOOST_CHECK_EQUAL(box, Box(8, 7, 15, 15));
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(IsWithin) {
	const Box b1{0, 0, 10, 10};
	BOOST_CHECK(b1.is_within(b1));
	BOOST_CHECK(Box(1, 1, 9, 9).is_within(b1));
	BOOST_CHECK(not Box(1, 1, 11, 10).is_within(b1));
	BOOST_CHECK(not Box(10, 10, 12, 12).is_within(b1));
}
