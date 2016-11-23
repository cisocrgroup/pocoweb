#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE ErrorTest

#include <boost/test/unit_test.hpp>
#include <iostream>
#include "core/Error.hpp"

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(Throw)
{
	BOOST_CHECK_THROW(throw BadRequest("BadRequest"), BadRequest);
}

