#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE LineTest

#include <boost/test/unit_test.hpp>
#include <functional>
#include <iostream>
#include <vector>
#include "core/Line.hpp"

using namespace pcw;

struct Fixture {
	static const char* gt;
	static const char* ocr;
	static const Box box;

	Fixture(): line(1, box) {
		line.append(ocr, 0, 100, 0.8);
		BOOST_REQUIRE(not line.empty());
	}
	Line line;
};

const char* Fixture::gt = "ꝑfectũ eſt: quod habet quidq◌̉d tempꝰ requirit ·";
const char* Fixture::ocr = "pectũ eſt: quoo habet quioo temp requirit";
const Box Fixture::box{0, 0, 100, 20};

////////////////////////////////////////////////////////////////////////////////
BOOST_FIXTURE_TEST_SUITE(LineTest, Fixture)

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(Basic)
{
	BOOST_CHECK_EQUAL(line.ocr(), ocr);
	BOOST_CHECK_EQUAL(line.cor(), ocr); // cor and ocr must be the same
	BOOST_CHECK_CLOSE(line.average_conf(), .8, .001); // it's not rocket sience
	BOOST_CHECK_EQUAL(line.chars().back().cut, 100);
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_SUITE_END()
