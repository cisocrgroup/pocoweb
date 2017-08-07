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

struct ProfileFixture {
	ProfileFixture()
	    : book(make_book()),
	      profile(book),
	      builder(book),
	      expressions(),
	      candidates() {
		expressions = {
		    {"thorm",
		     "thurm:{thurm+[(t:th,1)]}+ocr[(u:o,"
		     "3)],"
		     "voteWeight=0.03,levDistance=1"},
		    {"vnn",
		     "unn:{unn+[(nd:nn,1)]}+ocr[(u:v,0)]"
		     ","
		     "voteWeight=0.04,levDistance=1"},
		    {"maver",
		     "mauer:{mauer+[]}+ocr[(u:v,2)],"
		     "voteWeight=0.04,levDistance=1"},
		};
		for (const auto& e : expressions) {
			candidates[e.first] = Candidate(e.second);
		}
		book->find(1)->find(1)->each_token([this](const auto& token) {
			auto f = expressions.find(token.ocr());
			if (f != end(expressions)) {
				builder.add_candidate_string(token, f->second);
			}
		});
		profile = builder.build();
	}
	static ConstBookSptr make_book() {
		auto book = std::make_shared<Book>(1);
		auto page = std::make_shared<Page>(1);
		auto line = std::make_shared<Line>(1);
		line->append("maver thüre vnn vnn vnn thorm thorm", 0, 100,
			     0.8);
		page->push_back(line);
		book->push_back(*page);
		return book;
	}
	ConstBookSptr book;
	Profile profile;
	ProfileBuilder builder;
	std::map<std::string, std::string> expressions;
	std::map<std::string, Candidate> candidates;
};

////////////////////////////////////////////////////////////////////////////////
BOOST_FIXTURE_TEST_SUITE(ProfileTest, ProfileFixture)

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(Suggestions) {
	auto suggestions = profile.suggestions();
	BOOST_CHECK_EQUAL(suggestions.size(), 3);
	BOOST_CHECK(suggestions["thorm"].count(candidates["thorm"]));
	BOOST_CHECK(suggestions["maver"].count(candidates["maver"]));
	BOOST_CHECK(suggestions["vnn"].count(candidates["vnn"]));
	BOOST_CHECK(suggestions["thüre"].empty());
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(Counts) {
	BOOST_CHECK_EQUAL(profile.count("thorm"), 2);
	BOOST_CHECK_EQUAL(profile.count("maver"), 1);
	BOOST_CHECK_EQUAL(profile.count("vnn"), 3);
	BOOST_CHECK_EQUAL(profile.count("thüre"), 0);
	BOOST_CHECK_EQUAL(profile.count("something-else"), 0);
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(OcrPatternsUV) {
	auto ocrp = profile.calc_ocr_patterns();
	Pattern p("u", "v");
	BOOST_CHECK_EQUAL(ocrp.size(), 2);
	BOOST_REQUIRE(ocrp.count(p));
	BOOST_CHECK_EQUAL(ocrp[p].size(), 2);
	BOOST_CHECK(ocrp[p].count(Suggestion(candidates["maver"], "maver")));
	BOOST_CHECK(ocrp[p].count(Suggestion(candidates["vnn"], "vnn")));
	BOOST_CHECK(
	    not ocrp[p].count(Suggestion(candidates["thorm"], "thorm")));
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(OcrPatternsUO) {
	auto ocrp = profile.calc_ocr_patterns();
	Pattern p("u", "o");
	BOOST_CHECK_EQUAL(ocrp.size(), 2);
	BOOST_REQUIRE(ocrp.count(p));
	BOOST_CHECK_EQUAL(ocrp[p].size(), 1);
	BOOST_CHECK(
	    not ocrp[p].count(Suggestion(candidates["maver"], "maver")));
	BOOST_CHECK(not ocrp[p].count(Suggestion(candidates["vnn"], "vnn")));
	BOOST_CHECK(ocrp[p].count(Suggestion(candidates["thorm"], "thorm")));
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(OcrPatternsNDNN) {
	auto histp = profile.calc_hist_patterns();
	Pattern p("nd", "nn");
	BOOST_CHECK_EQUAL(histp.size(), 2);
	BOOST_REQUIRE(histp.count(p));
	BOOST_CHECK_EQUAL(histp[p].size(), 1);
	BOOST_CHECK(
	    not histp[p].count(Suggestion(candidates["maver"], "maver")));
	BOOST_CHECK(histp[p].count(Suggestion(candidates["vnn"], "vnn")));
	BOOST_CHECK(
	    not histp[p].count(Suggestion(candidates["thorm"], "thorm")));
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(OcrPatternsTTH) {
	auto histp = profile.calc_hist_patterns();
	Pattern p("t", "th");
	BOOST_CHECK_EQUAL(histp.size(), 2);
	BOOST_REQUIRE(histp.count(p));
	BOOST_CHECK_EQUAL(histp[p].size(), 1);
	BOOST_CHECK(
	    not histp[p].count(Suggestion(candidates["maver"], "maver")));
	BOOST_CHECK(not histp[p].count(Suggestion(candidates["vnn"], "vnn")));
	BOOST_CHECK(histp[p].count(Suggestion(candidates["thorm"], "thorm")));
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
