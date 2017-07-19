#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE JsonTest

#include <crow/json.h>
#include <boost/test/unit_test.hpp>
#include <iostream>
#include "core/jsonify.hpp"

using namespace pcw;
static const char* data =
    R"({"fourdottwo":4.2,"fifteen":15,"true":true,"false":false,"string":"string"})";
struct Fixture {
	RJson json;
	Fixture() { json = crow::json::load(data); }
};

////////////////////////////////////////////////////////////////////////////////
BOOST_FIXTURE_TEST_SUITE(LineBuilder, Fixture)

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(GetStringTest) {
	BOOST_REQUIRE(get<std::string>(json, "string"));
	BOOST_CHECK_EQUAL("string", *get<std::string>(json, "string"));
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(GetIntTest) {
	BOOST_REQUIRE(get<int>(json, "fifteen"));
	BOOST_CHECK_EQUAL(15, *get<int>(json, "fifteen"));
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(GetDoubleTest) {
	BOOST_REQUIRE(get<double>(json, "fourdottwo"));
	BOOST_CHECK_EQUAL(4.2, *get<double>(json, "fourdottwo"));
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(GetFalseTest) {
	BOOST_REQUIRE(get<bool>(json, "false"));
	BOOST_CHECK_EQUAL(false, *get<bool>(json, "false"));
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(GetTrueTest) {
	BOOST_REQUIRE(get<bool>(json, "true"));
	BOOST_CHECK_EQUAL(true, *get<bool>(json, "true"));
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(NotFoundTest) {
	BOOST_CHECK_EQUAL(static_cast<bool>(get<int>(json, "nothing")), false);
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(IfsetStringTest) {
	bool called = false;
	ifset<std::string>(json, "string", [&](const auto& str) {
		BOOST_CHECK_EQUAL(str, "string");
		called = true;
	});
	BOOST_CHECK_EQUAL(called, true);
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(IfsetDoubleTest) {
	bool called = false;
	ifset<double>(json, "fourdottwo", [&](const auto& d) {
		BOOST_CHECK_EQUAL(d, 4.2);
		called = true;
	});
	BOOST_CHECK_EQUAL(called, true);
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(IfsetIntTest) {
	bool called = false;
	ifset<int>(json, "fifteen", [&](const auto& i) {
		BOOST_CHECK_EQUAL(i, 15);
		called = true;
	});
	BOOST_CHECK_EQUAL(called, true);
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(IfsetTrueTest) {
	bool called = false;
	ifset<bool>(json, "true", [&](const auto& b) {
		BOOST_CHECK_EQUAL(b, true);
		called = true;
	});
	BOOST_CHECK_EQUAL(called, true);
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(IfsetFalseTest) {
	bool called = false;
	ifset<bool>(json, "false", [&](const auto& b) {
		BOOST_CHECK_EQUAL(b, false);
		called = true;
	});
	BOOST_CHECK_EQUAL(called, true);
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(IfSetNotCalledTest) {
	bool called = false;
	ifset<int>(json, "not-found", [&](const auto&) {
		called = true;
		BOOST_REQUIRE(false);
	});
	BOOST_CHECK_EQUAL(called, false);
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_SUITE_END()
