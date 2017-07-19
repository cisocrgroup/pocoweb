#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE LineTest

#include <boost/test/unit_test.hpp>
#include <iostream>
#include "core/Line.hpp"

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_SUITE(LineTest)

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(SimpleAppend) {
	const auto line = std::make_shared<Line>(1, Box{0, 0, 50, 10});
	line->append(L'a', 10, 0);
	line->append(L'b', 10, 0);
	line->append(L' ', 10, 0);
	line->append(L'c', 10, 0);
	line->append(L'd', 10, 0);
	const auto tokens = line->tokens();
	BOOST_REQUIRE(tokens.size() == 3);
	BOOST_CHECK_EQUAL(tokens[0].cor(), "ab");
	BOOST_CHECK_EQUAL(tokens[1].cor(), " ");
	BOOST_CHECK_EQUAL(tokens[2].cor(), "cd");
	BOOST_CHECK_EQUAL(tokens[0].box.width(), 20);
	BOOST_CHECK_EQUAL(tokens[1].box.width(), 10);
	BOOST_CHECK_EQUAL(tokens[2].box.width(), 20);
	BOOST_CHECK_EQUAL(tokens[0].box.left(), 0);
	BOOST_CHECK_EQUAL(tokens[0].box.right(), 20);
	BOOST_CHECK_EQUAL(tokens[0].box.left(), 20);
	BOOST_CHECK_EQUAL(tokens[0].box.right(), 30);
	BOOST_CHECK_EQUAL(tokens[0].box.left(), 30);
	BOOST_CHECK_EQUAL(tokens[0].box.right(), 50);
	for (const auto& token : tokens) {
		BOOST_CHECK_EQUAL(token.box.top(), line->box.top());
		BOOST_CHECK_EQUAL(token.box.bottom(), line->box.bottom());
	}
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_SUITE_END()
