#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE TestConnectionPool

#include <boost/test/unit_test.hpp>
#include <sqlpp11/sqlpp11.h>
#include "utils/MockDb.h"
#include "database/ConnectionPool.hpp"

using namespace sqlpp;
using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
// Check if pool compiles with not additional arguments
BOOST_AUTO_TEST_CASE(CompilesWithNoArgs)
{
	ConnectionPool<MockDb> pool(5);
}

struct Mock {
	Mock(int a, double b): i(a), d(b) /* :) */ {}
	int i;
	double d; // :)
};

struct Fixture {
	ConnectionPool<Mock, int, double> pool;
	Fixture(): pool(3, 13, 4.2) {}
};

////////////////////////////////////////////////////////////////////////////////
BOOST_FIXTURE_TEST_SUITE(ConnectionPool, Fixture)

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(Exhausts)
{
	auto c1 = pool.get_connection();
	auto c2 = pool.get_connection();
	auto c3 = pool.get_connection();
	auto c4 = pool.get_connection();

	BOOST_REQUIRE(c1);
	BOOST_CHECK_EQUAL(c1.db().i, 13);
	BOOST_CHECK_EQUAL(c1.db().d, 4.2);
	BOOST_REQUIRE(c2);
	BOOST_CHECK_EQUAL(c2.db().i, 13);
	BOOST_CHECK_EQUAL(c2.db().d, 4.2);
	BOOST_REQUIRE(c3);
	BOOST_CHECK_EQUAL(c3.db().i, 13);
	BOOST_CHECK_EQUAL(c3.db().d, 4.2);
	BOOST_CHECK(not c4);
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(Release)
{
	auto c1 = pool.get_connection();
	BOOST_CHECK(c1);
	auto c2 = pool.get_connection();
	BOOST_CHECK(c2);
	{
		auto c3 = pool.get_connection();
		BOOST_CHECK(c3);
		auto c4 = pool.get_connection();
		BOOST_CHECK(not c4);
	}
	auto c5 = pool.get_connection();
	BOOST_CHECK(c5);
	auto c6 = pool.get_connection();
	BOOST_CHECK(not c6);
	c1 = nullptr;
	auto c7 = pool.get_connection();
	BOOST_CHECK(not c7);
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_SUITE_END()
