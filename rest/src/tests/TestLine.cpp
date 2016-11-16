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

const char* Fixture::gt = "ꝑfectũ eſt: quidq◌̉d tempꝰ ·";
const char* Fixture::ocr = "pectũeſt: quioo te mp";
const Box Fixture::box{0, 0, 100, 20};

////////////////////////////////////////////////////////////////////////////////
BOOST_FIXTURE_TEST_SUITE(LineTest, Fixture)

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(Ocr)
{
	BOOST_CHECK_EQUAL(line.ocr(), ocr);
	BOOST_CHECK_EQUAL(line.cor(), ocr); // cor and ocr must be the same
	BOOST_CHECK_CLOSE(line.average_conf(), .8, .001); // it's not rocket sience
	BOOST_CHECK_EQUAL(line.chars().back().cut, 100);
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(OcrTokens)
{
	auto tokens = line.tokens();
	BOOST_REQUIRE(tokens.size() == 7);

	BOOST_CHECK(tokens[0].is_normal());
	BOOST_CHECK_EQUAL(tokens[0].ocr(), "pectũeſt"); 
	BOOST_CHECK_EQUAL(tokens[0].cor(), "pectũeſt"); 

	BOOST_CHECK(not tokens[1].is_normal());
	BOOST_CHECK_EQUAL(tokens[1].ocr(), ": ");
	BOOST_CHECK_EQUAL(tokens[1].cor(), ": ");

	BOOST_CHECK(tokens[2].is_normal());
	BOOST_CHECK_EQUAL(tokens[2].ocr(), "quioo"); 
	BOOST_CHECK_EQUAL(tokens[2].cor(), "quioo"); 

	BOOST_CHECK(not tokens[3].is_normal());
	BOOST_CHECK_EQUAL(tokens[3].ocr(), " "); 
	BOOST_CHECK_EQUAL(tokens[3].cor(), " "); 

	BOOST_CHECK(tokens[4].is_normal());
	BOOST_CHECK_EQUAL(tokens[4].ocr(), "te"); 
	BOOST_CHECK_EQUAL(tokens[4].cor(), "te"); 

	BOOST_CHECK(not tokens[5].is_normal());
	BOOST_CHECK_EQUAL(tokens[5].ocr(), " "); 
	BOOST_CHECK_EQUAL(tokens[5].cor(), " "); 

	BOOST_CHECK(tokens[6].is_normal());
	BOOST_CHECK_EQUAL(tokens[6].ocr(), "mp"); 
	BOOST_CHECK_EQUAL(tokens[6].cor(), "mp"); 
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_SUITE_END()
