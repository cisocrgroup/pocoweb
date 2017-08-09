#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE ProfileTest

#include <boost/test/unit_test.hpp>
#include <iostream>
#include "core/Book.hpp"
#include "core/Line.hpp"
#include "core/Page.hpp"
#include "profiler/Profile.hpp"
#include "profiler/docxml.hpp"
#include "utils/Error.hpp"
#include "utils/TmpDir.hpp"

using namespace pcw;

struct CandidateFixture {
	CandidateFixture()
	    : cand(
		  "μυῶνος:{μυῶνος+[(a:b,3)]}+ocr[(υῶ:ε,1)(c:d,2)],"
		  "voteWeight=0.0285641,"
		  "levDistance=2") {}
	Candidate cand;
};

////////////////////////////////////////////////////////////////////////////////
BOOST_FIXTURE_TEST_SUITE(CandidateTest, CandidateFixture)

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(Candidate) {
	BOOST_CHECK_EQUAL(cand.cor(), "μυῶνος");
	BOOST_CHECK_CLOSE_FRACTION(cand.weight(), 0.0285641, 0.0001);
	BOOST_CHECK_EQUAL(cand.lev(), 2);
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(CandidateError) {
	BOOST_CHECK_THROW(pcw::Candidate tmp("invalid:{foo+[}+ocr[],"),
			  ParseError);
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(Explanation) {
	auto e = cand.explanation();
	BOOST_CHECK_EQUAL(e.hist, "μυῶνος");

	// hist patterns
	BOOST_REQUIRE_EQUAL(e.histp.patterns.size(), 1U);
	BOOST_CHECK_EQUAL(e.histp.patterns[0].left, "a");
	BOOST_CHECK_EQUAL(e.histp.patterns[0].right, "b");
	BOOST_CHECK_EQUAL(e.histp.patterns[0].pos, 3U);

	// ocr patterns
	BOOST_REQUIRE_EQUAL(e.ocrp.patterns.size(), 2U);
	BOOST_CHECK_EQUAL(e.ocrp.patterns[0].left, "υῶ");
	BOOST_CHECK_EQUAL(e.ocrp.patterns[0].right, "ε");
	BOOST_CHECK_EQUAL(e.ocrp.patterns[0].pos, 1U);
	BOOST_CHECK_EQUAL(e.ocrp.patterns[1].left, "c");
	BOOST_CHECK_EQUAL(e.ocrp.patterns[1].right, "d");
	BOOST_CHECK_EQUAL(e.ocrp.patterns[1].pos, 2U);
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(TestIntegerVoteWeight) {
	cand = pcw::Candidate(
	    "Homine:{homine+[]}+ocr[(h:b,0)],voteWeight=1,levDistance=1");
	BOOST_CHECK_EQUAL(cand.cor(), "Homine");
	BOOST_CHECK_EQUAL(cand.lev(), 1);
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(TestScientificVoteWeight) {
	cand = pcw::Candidate(
	    "moſtra:{mostra+[(s:ſ,2)]}+ocr[(m:n,0)],voteWeight=3.97962e-10,"
	    "levDistance=1");
	BOOST_CHECK_EQUAL(cand.cor(), "moſtra");
	BOOST_CHECK_EQUAL(cand.lev(), 1);
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_SUITE_END()
