#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE ProfileTest

#include <boost/test/unit_test.hpp>
#include <iostream>
#include "core/Error.hpp"
#include "core/Profile.hpp"

using namespace pcw;

struct CandidateFixture {
	CandidateFixture()
		: cand("μυῶνος:{μυῶνος+[(a:b,3)]}+ocr[(υῶ:ε,1)(c:d,2)],"
				"voteWeight=0.0285641,"
				"levDistance=2")
	{}
	Candidate cand;
};

////////////////////////////////////////////////////////////////////////////////
BOOST_FIXTURE_TEST_SUITE(CandidateTest, CandidateFixture)

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(Candidate)
{
	BOOST_CHECK_EQUAL(cand.cor(), "μυῶνος");
	BOOST_CHECK_CLOSE_FRACTION(cand.weight(), 0.0285641, 0.0001);
	BOOST_CHECK_EQUAL(cand.lev(), 2);
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(CandidateError)
{
	BOOST_CHECK_THROW(pcw::Candidate tmp("invalid:{foo+[}+ocr[],"), ParseError);
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(Explanation)
{
	auto e = cand.explanation();
	BOOST_CHECK_EQUAL(e.hist, "μυῶνος");

	// hist patterns
	BOOST_REQUIRE_EQUAL(e.histp.patterns.size(), 1U);
	BOOST_CHECK_EQUAL(e.histp.patterns[0].cor, "a");
	BOOST_CHECK_EQUAL(e.histp.patterns[0].ocr, "b");
	BOOST_CHECK_EQUAL(e.histp.patterns[0].pos, 3U);

	// ocr patterns
	BOOST_REQUIRE_EQUAL(e.ocrp.patterns.size(), 2U);
	BOOST_CHECK_EQUAL(e.ocrp.patterns[0].cor, "υῶ");
	BOOST_CHECK_EQUAL(e.ocrp.patterns[0].ocr, "ε");
	BOOST_CHECK_EQUAL(e.ocrp.patterns[0].pos, 1U);
	BOOST_CHECK_EQUAL(e.ocrp.patterns[1].cor, "c");
	BOOST_CHECK_EQUAL(e.ocrp.patterns[1].ocr, "d");
	BOOST_CHECK_EQUAL(e.ocrp.patterns[1].pos, 2U);
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_SUITE_END()

