#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE UniqueIdMapTest

#include <boost/test/unit_test.hpp>
#include <iostream>
#include "utils/UniqueIdMap.hpp"

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(UniqueIds) {
	UniqueIdMap<std::string> ids;
	BOOST_CHECK_EQUAL(ids["first"], 1);
	BOOST_CHECK_EQUAL(ids["second"], 2);
	BOOST_CHECK_EQUAL(ids["third"], 3);
	BOOST_CHECK_EQUAL(ids["first"], 1);
	BOOST_CHECK_EQUAL(ids["second"], 2);
	BOOST_CHECK_EQUAL(ids["third"], 3);
}
