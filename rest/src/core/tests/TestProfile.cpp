#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE ProfileTest

#include <boost/test/unit_test.hpp>
#include <iostream>
#include "core/Book.hpp"
#include "core/Line.hpp"
#include "core/Page.hpp"
#include "core/Profile.hpp"
#include "core/docxml.hpp"
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
BOOST_AUTO_TEST_CASE(TestEmptyHistPattern) {
	cand = Candidate(
	    "Homine:{homine+[]}+ocr[(h:b,0)],voteWeight=1,levDistance=1");
	BOOST_CHECK_EQUAL(cand.explanation().hist, "homine");
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_SUITE_END()

struct ProfileFixture {
	ProfileFixture() : book(make_book()), profile(book), builder(book) {
		book->find(1)->find(1)->each_token([this](const auto& token) {
			if (token.ocr() == "thorm")
				builder.add_candidate_string(
				    token,
				    "thurm:{thurm+[(t:th,1)]}+ocr[(u:o,"
				    "3)],"
				    "voteWeight=0.03,levDistance=1");
			else if (token.ocr() == "vnn")
				builder.add_candidate_string(
				    token,
				    "unn:{unn+[(nd:nn,1)]}+ocr[(u:v,0)]"
				    ","
				    "voteWeight=0.04,levDistance=1");
			else if (token.ocr() == "maver")
				builder.add_candidate_string(
				    token,
				    "mauer:{mauer+[]}+ocr[(u:v,2)],"
				    "voteWeight=0.04,levDistance=1");
		});
		profile = builder.build();
	}
	static ConstBookSptr make_book() {
		auto book = std::make_shared<Book>(1);
		auto page = std::make_shared<Page>(1);
		auto line = std::make_shared<Line>(1);
		line->append("maver thüre vnn thorm", 0, 100, 0.8);
		page->push_back(line);
		book->push_back(*page);
		return book;
	}
	static bool contains(const Profile::Suggestions& s, const char* cor,
			     const char* ocr) {
		return std::any_of(begin(s), end(s), [ocr, cor](const auto& s) {
			return s.cand.cor() == cor and s.token.ocr() == ocr;
		});
	}
	ConstBookSptr book;
	Profile profile;
	ProfileBuilder builder;
};

////////////////////////////////////////////////////////////////////////////////
BOOST_FIXTURE_TEST_SUITE(ProfileTest, ProfileFixture)

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(Suggestions) {
	BOOST_CHECK_EQUAL(profile.suggestions().size(), 3);
	BOOST_CHECK(contains(profile.suggestions(), "thurm", "thorm"));
	BOOST_CHECK(contains(profile.suggestions(), "mauer", "maver"));
	BOOST_CHECK(contains(profile.suggestions(), "unn", "vnn"));
	BOOST_CHECK(not contains(profile.suggestions(), "thüre", "thüre"));
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(OcrPatternsUV) {
	auto ocrp = profile.calc_ocr_patterns();
	Pattern p("u", "v");
	BOOST_CHECK_EQUAL(ocrp.size(), 2);
	BOOST_REQUIRE(ocrp.count(p));
	BOOST_CHECK_EQUAL(ocrp[p].size(), 2);
	BOOST_CHECK(contains(ocrp[p], "mauer", "maver"));
	BOOST_CHECK(contains(ocrp[p], "unn", "vnn"));
	BOOST_CHECK(not contains(ocrp[p], "thurm", "thorm"));
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(OcrPatternsUO) {
	auto ocrp = profile.calc_ocr_patterns();
	Pattern p("u", "o");
	BOOST_CHECK_EQUAL(ocrp.size(), 2);
	BOOST_REQUIRE(ocrp.count(p));
	BOOST_CHECK_EQUAL(ocrp[p].size(), 1);
	BOOST_CHECK(contains(ocrp[p], "thurm", "thorm"));
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(OcrPatternsNDNN) {
	auto histp = profile.calc_hist_patterns();
	Pattern p("nd", "nn");
	BOOST_CHECK_EQUAL(histp.size(), 2);
	BOOST_REQUIRE(histp.count(p));
	BOOST_CHECK_EQUAL(histp[p].size(), 1);
	BOOST_CHECK(contains(histp[p], "unn", "vnn"));
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(OcrPatternsTTH) {
	auto histp = profile.calc_hist_patterns();
	Pattern p("t", "th");
	BOOST_CHECK_EQUAL(histp.size(), 2);
	BOOST_REQUIRE(histp.count(p));
	BOOST_CHECK_EQUAL(histp[p].size(), 1);
	BOOST_CHECK(contains(histp[p], "thurm", "thorm"));
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(PatternsIO) {
	TmpDir tmp;
	auto file = tmp / "doc.xml";

	// write
	DocXml docxml;
	docxml << profile;
	docxml.save_file(file.string().data());

	// read again
	builder = ProfileBuilder(book);
	builder.add_candidates_from_file(file);
	profile = builder.build();

	BOOST_CHECK_EQUAL(profile.suggestions().size(), 3);
	BOOST_CHECK_EQUAL(profile.calc_ocr_patterns().size(), 2);
	BOOST_CHECK_EQUAL(profile.calc_hist_patterns().size(), 2);
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_SUITE_END()
