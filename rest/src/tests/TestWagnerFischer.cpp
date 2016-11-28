#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE WagnerFischerTest

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

	Fixture(): line(std::make_shared<Line>(1)), wf() {
		line->append(ocr, 0, 100, 0.8);
		BOOST_REQUIRE(not line->empty());
		wf.set_gt(gt);
		wf.set_ocr(*line);
		BOOST_CHECK_EQUAL(wf(), 4);
	}
	LinePtr line;
	WagnerFischer wf;
};

const char* Fixture::gt  = "Dum fata ſinunt, vivite laeti.";
const char* Fixture::ocr = "Dum fa ta ſi unt, ivite laet.";

////////////////////////////////////////////////////////////////////////////////
BOOST_FIXTURE_TEST_SUITE(WagnerFischerTest, Fixture)

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(Full)
{
	wf.correct(*line);
	BOOST_CHECK_EQUAL(line->ocr(), ocr);
	BOOST_CHECK_EQUAL(line->cor(), gt);
	BOOST_CHECK(line->is_corrected());
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(Partial)
{
	wf.correct(*line, true); // correct partial
	BOOST_CHECK_EQUAL(line->ocr(), ocr);
	BOOST_CHECK_EQUAL(line->cor(), gt);
	BOOST_CHECK(not line->is_corrected());

	auto words = line->words();
	BOOST_REQUIRE_EQUAL(words.size(), 5);
	BOOST_CHECK(not words[0].is_corrected());
	BOOST_CHECK_EQUAL(words[0].cor(), "Dum");
	BOOST_CHECK(words[1].is_corrected());
	BOOST_CHECK_EQUAL(words[1].cor(), "fata");
	BOOST_CHECK(words[2].is_corrected());
	BOOST_CHECK_EQUAL(words[2].cor(), "ſinunt");
	BOOST_CHECK(words[3].is_corrected());
	BOOST_CHECK_EQUAL(words[3].cor(), "vivite");
	BOOST_CHECK(words[4].is_corrected());
	BOOST_CHECK_EQUAL(words[4].cor(), "laeti");
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(Pattern)
{
	wf.clear();
	wf.set_ocr(ocr);
	wf.set_gt(std::wregex{L"fa ta"}, L"fata");
	wf.set_gt(std::wregex{L"ſi unt"}, L"ſinunt");
	BOOST_CHECK_EQUAL(wf(), 2);
	wf.correct(*line, true); // correct partial
	BOOST_CHECK(not line->is_corrected());

	auto words = line->words();
	BOOST_REQUIRE_EQUAL(words.size(), 5);
	BOOST_CHECK(not words[0].is_corrected());
	BOOST_CHECK_EQUAL(words[0].cor(), "Dum");
	BOOST_CHECK(words[1].is_corrected());
	BOOST_CHECK_EQUAL(words[1].cor(), "fata");
	BOOST_CHECK(words[2].is_corrected());
	BOOST_CHECK_EQUAL(words[2].cor(), "ſinunt");
	BOOST_CHECK(not words[3].is_corrected());
	BOOST_CHECK_EQUAL(words[3].cor(), "ivite");
	BOOST_CHECK(not words[4].is_corrected());
	BOOST_CHECK_EQUAL(words[4].cor(), "laet");
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(Bug1)
{
	wf.set_gt("De Homine.");
	wf.set_ocr("$=");
	BOOST_CHECK_EQUAL(wf(), 10);
	std::stringstream os;
	os << wf.trace();
	auto trace = os.str();
	BOOST_CHECK_EQUAL(trace, "--------##");
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_SUITE_END()
