#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE QueriesTest

#include <crow.h>
#include <boost/test/unit_test.hpp>
#include <iostream>
#include "core/queries.hpp"

using namespace pcw;

struct Fixture {
	Query query;
	Fixture()
	    : query(
		  "http://"
		  "example.org?"
		  "string=string&int=13&double=2.7&true=1&false=0") {}
};

////////////////////////////////////////////////////////////////////////////////
BOOST_FIXTURE_TEST_SUITE(QueriesTest, Fixture)

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(TestStringQuery) {
	BOOST_REQUIRE(query_get<std::string>(query, "string"));
	BOOST_CHECK_EQUAL(*query_get<std::string>(query, "string"), "string");
	BOOST_REQUIRE(query_get<std::string>(query, "int"));
	BOOST_CHECK_EQUAL(*query_get<std::string>(query, "int"), "13");
	BOOST_REQUIRE(query_get<std::string>(query, "double"));
	BOOST_CHECK_EQUAL(*query_get<std::string>(query, "double"), "2.7");
	BOOST_REQUIRE(query_get<std::string>(query, "false"));
	BOOST_CHECK_EQUAL(*query_get<std::string>(query, "false"), "0");
	BOOST_REQUIRE(query_get<std::string>(query, "true"));
	BOOST_CHECK_EQUAL(*query_get<std::string>(query, "true"), "1");
	BOOST_CHECK(not query_get<std::string>(query, "another-string"));
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(TestIntQuery) {
	BOOST_REQUIRE(query_get<int>(query, "int"));
	BOOST_CHECK_EQUAL(*query_get<int>(query, "int"), 13);
	BOOST_REQUIRE(query_get<int>(query, "true"));
	BOOST_CHECK_EQUAL(*query_get<int>(query, "true"), 1);
	BOOST_REQUIRE(query_get<int>(query, "false"));
	BOOST_CHECK_EQUAL(*query_get<int>(query, "false"), 0);
	BOOST_CHECK(not query_get<int>(query, "double"));
	BOOST_CHECK(not query_get<int>(query, "string"));
	BOOST_CHECK(not query_get<int>(query, "not-an-int"));
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(TestDoubleQuery) {
	BOOST_REQUIRE(query_get<double>(query, "double"));
	BOOST_CHECK_EQUAL(*query_get<double>(query, "double"), 2.7);
	BOOST_REQUIRE(query_get<double>(query, "int"));
	BOOST_CHECK_EQUAL(*query_get<double>(query, "int"), 13.0);
	BOOST_REQUIRE(query_get<int>(query, "true"));
	BOOST_CHECK_EQUAL(*query_get<int>(query, "true"), 1);
	BOOST_REQUIRE(query_get<int>(query, "false"));
	BOOST_CHECK_EQUAL(*query_get<int>(query, "false"), 0);
	BOOST_CHECK(not query_get<double>(query, "string"));
	BOOST_CHECK(not query_get<double>(query, "not-a-double"));
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(TestBoolQuery) {
	BOOST_REQUIRE(query_get<bool>(query, "true"));
	BOOST_CHECK_EQUAL(*query_get<bool>(query, "true"), true);
	BOOST_REQUIRE(query_get<bool>(query, "false"));
	BOOST_CHECK_EQUAL(*query_get<bool>(query, "false"), false);
	BOOST_CHECK(not query_get<bool>(query, "int"));
	BOOST_CHECK(not query_get<bool>(query, "double"));
	BOOST_CHECK(not query_get<bool>(query, "string"));
	BOOST_CHECK(not query_get<bool>(query, "not-a-bool"));
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_SUITE_END()
