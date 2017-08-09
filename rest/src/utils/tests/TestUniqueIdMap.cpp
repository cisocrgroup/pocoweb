#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE UniqueIdMapTest

#include <boost/test/unit_test.hpp>
#include <iostream>
#include "utils/UniqueIdMap.hpp"

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(UniqueIds) {
	UniqueIdMap<std::string> ids;
	BOOST_CHECK_EQUAL(ids["first"].first, 1);
	BOOST_CHECK_EQUAL(ids["second"].first, 2);
	BOOST_CHECK_EQUAL(ids["third"].first, 3);
	BOOST_CHECK_EQUAL(ids["first"].first, 1);
	BOOST_CHECK_EQUAL(ids["second"].first, 2);
	BOOST_CHECK_EQUAL(ids["third"].first, 3);
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(NewIds) {
	UniqueIdMap<std::string> ids;
	BOOST_CHECK_EQUAL(ids["first"].second, true);
	BOOST_CHECK_EQUAL(ids["second"].second, true);
	BOOST_CHECK_EQUAL(ids["third"].second, true);
	BOOST_CHECK_EQUAL(ids["first"].second, false);
	BOOST_CHECK_EQUAL(ids["second"].second, false);
	BOOST_CHECK_EQUAL(ids["third"].second, false);
}
