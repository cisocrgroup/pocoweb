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

	Fixture(): line(1), wf() {
		line.append(ocr, 0, 100, 0.8);
		BOOST_REQUIRE(not line.empty());
		wf.set_gt(gt);
		wf.set_ocr(line);
		BOOST_CHECK_EQUAL(wf(), 4);
		wf.correct(line);
	}
	Line line;
	WagnerFischer wf;
};

const char* Fixture::gt  = "Dum fata ſinunt, vivite laeti.";
const char* Fixture::ocr = "Dum fa ta ſi unt, ivite laet.";

////////////////////////////////////////////////////////////////////////////////
BOOST_FIXTURE_TEST_SUITE(WagnerFischerTest, Fixture)

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(Ocr)
{
	BOOST_CHECK_EQUAL(line.ocr(), ocr);
	BOOST_CHECK_EQUAL(line.cor(), gt);
	BOOST_CHECK(line.is_corrected());
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_SUITE_END()
