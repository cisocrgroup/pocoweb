#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE CorLineTest

#include <boost/test/unit_test.hpp>
#include <functional>
#include <iostream>
#include <vector>
#include "core/Line.hpp"
#include "core/WagnerFischer.hpp"

using namespace pcw;

struct Fixture {
	static const char* gt;
	static const char* ocr;
	static const Box box;

	Fixture(): line(1, box), wf() {
		line.append(ocr, 0, 100, 0.8);
		BOOST_REQUIRE(not line.empty());
		wf.set_gt(gt);
		wf.set_ocr(line);
		BOOST_CHECK_EQUAL(wf(), 11);
		wf.correct(line);
	}
	Line line;
	WagnerFischer wf;
};

const char* Fixture::gt = "ꝑfectũ eſt: quidq̉d tempꝰ ·";
const char* Fixture::ocr = "pectũeſt: quioo te mp";
const Box Fixture::box{0, 0, 100, 20};

////////////////////////////////////////////////////////////////////////////////
BOOST_FIXTURE_TEST_SUITE(CorLineTest, Fixture)

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(Ocr)
{
	BOOST_CHECK_EQUAL(line.ocr(), ocr);
	BOOST_CHECK_EQUAL(line.cor(), gt);
	BOOST_CHECK_EQUAL(line.chars().back().cut, 100);
	BOOST_CHECK(line.average_conf() > .8);
	BOOST_CHECK(line.is_corrected());
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(Tokens)
{
	auto tokens = line.tokens();
	BOOST_REQUIRE_EQUAL(tokens.size(), 8);

	BOOST_CHECK(tokens[0].is_normal());
	BOOST_CHECK_EQUAL(tokens[0].cor(), "ꝑfectũ");
	BOOST_CHECK(tokens[0].is_corrected());

	BOOST_CHECK(not tokens[1].is_normal());
	BOOST_CHECK_EQUAL(tokens[1].cor(), " ");
	BOOST_CHECK(tokens[1].is_corrected());

	BOOST_CHECK(tokens[2].is_normal());
	BOOST_CHECK_EQUAL(tokens[2].cor(), "eſt");
	BOOST_CHECK(tokens[2].is_corrected());

	BOOST_CHECK(not tokens[3].is_normal());
	BOOST_CHECK_EQUAL(tokens[3].cor(), ": ");
	BOOST_CHECK(tokens[3].is_corrected());

	BOOST_CHECK(tokens[4].is_normal());
	BOOST_CHECK_EQUAL(tokens[4].cor(), "quidq̉d");
	BOOST_CHECK(tokens[4].is_corrected());

	BOOST_CHECK(not tokens[5].is_normal());
	BOOST_CHECK_EQUAL(tokens[5].cor(), " ");
	BOOST_CHECK(tokens[5].is_corrected());

	BOOST_CHECK(tokens[6].is_normal());
	BOOST_CHECK_EQUAL(tokens[6].cor(), "tempꝰ");
	BOOST_CHECK(tokens[6].is_corrected());

	BOOST_CHECK(not tokens[7].is_normal());
	BOOST_CHECK_EQUAL(tokens[7].cor(), " ·");
	BOOST_CHECK(tokens[7].is_corrected());
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(Words)
{
	auto words = line.words();
	BOOST_REQUIRE_EQUAL(words.size(), 4);

	BOOST_CHECK(words[0].is_normal());
	BOOST_CHECK_EQUAL(words[0].cor(), "ꝑfectũ");
	BOOST_CHECK(words[0].is_corrected());

	BOOST_CHECK(words[1].is_normal());
	BOOST_CHECK_EQUAL(words[1].cor(), "eſt");
	BOOST_CHECK(words[1].is_corrected());

	BOOST_CHECK(words[2].is_normal());
	BOOST_CHECK_EQUAL(words[2].cor(), "quidq̉d");
	BOOST_CHECK(words[2].is_corrected());

	BOOST_CHECK(words[3].is_normal());
	BOOST_CHECK_EQUAL(words[3].cor(), "tempꝰ");
	BOOST_CHECK(words[3].is_corrected());
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_SUITE_END()
