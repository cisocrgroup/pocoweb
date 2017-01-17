#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE BoxTest

#include <boost/test/unit_test.hpp>
#include <iostream>
#include "core/Box.hpp"

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(Left)
{
	Box box{1};
	BOOST_CHECK_EQUAL(box.left(), 1);
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(Top)
{
	Box box{0, 1};
	BOOST_CHECK_EQUAL(box.top(), 1);
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(Right)
{
	Box box{0, 0, 1};
	BOOST_CHECK_EQUAL(box.right(), 1);
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(Bottom)
{
	Box box{0, 0, 0, 1};
	BOOST_CHECK_EQUAL(box.bottom(), 1);
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(Width)
{
	Box box{1, 0, 2, 0};
	BOOST_CHECK_EQUAL(box.width(), 1);
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(Height)
{
	Box box{0, 1, 0, 2};
	BOOST_CHECK_EQUAL(box.height(), 1);
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(Invalid)
{
	Box box{-1, -2, -3, -4};
	BOOST_CHECK_EQUAL(box.width(), 0);
	BOOST_CHECK_EQUAL(box.height(), 0);
	BOOST_CHECK_EQUAL(box.left(), 0);
	BOOST_CHECK_EQUAL(box.top(), 0);
	BOOST_CHECK_EQUAL(box.right(), 0);
	BOOST_CHECK_EQUAL(box.bottom(), 0);
}

