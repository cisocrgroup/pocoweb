#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE WagnerFischerTest

#include "core/Line.hpp"
#include "core/WagnerFischer.hpp"
#include "core/util.hpp"
#include <boost/test/unit_test.hpp>
#include <functional>
#include <iostream>
#include <vector>

using namespace pcw;

struct Fixture {
  static const char *gt;
  static const char *ocr;

  Fixture() : line(std::make_shared<Line>(1)), wf() {
    line->append(ocr, 0, 100, 0.8);
    BOOST_REQUIRE(not line->empty());
    wf.set_gt(gt);
    wf.set_ocr(*line);
    BOOST_CHECK_EQUAL(wf(), 4);
  }
  LinePtr line;
  WagnerFischer wf;
};

const char *Fixture::gt = "Dum fata ſinunt, vivite laeti.";
const char *Fixture::ocr = "Dum fa ta ſi unt, ivite laet.";

////////////////////////////////////////////////////////////////////////////////
BOOST_FIXTURE_TEST_SUITE(WagnerFischerTest, Fixture)

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(Full) {
  wf.correct(*line);
  BOOST_CHECK_EQUAL(line->ocr(), ocr);
  BOOST_CHECK_EQUAL(line->cor(), gt);
  BOOST_CHECK(line->is_fully_corrected());
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(Partial) {
  wf.correct(*line, true); // correct partial
  BOOST_CHECK_EQUAL(line->ocr(), ocr);
  BOOST_CHECK_EQUAL(line->cor(), gt);
  BOOST_CHECK(not line->is_fully_corrected());

  auto words = line->words();
  BOOST_REQUIRE_EQUAL(words.size(), 5);
  BOOST_CHECK(not words[0].is_fully_corrected());
  BOOST_CHECK_EQUAL(words[0].cor(), "Dum");
  BOOST_CHECK(words[1].is_fully_corrected());
  BOOST_CHECK_EQUAL(words[1].cor(), "fata");
  BOOST_CHECK(words[2].is_fully_corrected());
  BOOST_CHECK_EQUAL(words[2].cor(), "ſinunt");
  BOOST_CHECK(words[3].is_fully_corrected());
  BOOST_CHECK_EQUAL(words[3].cor(), "vivite");
  BOOST_CHECK(words[4].is_fully_corrected());
  BOOST_CHECK_EQUAL(words[4].cor(), "laeti");
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(Pattern) {
  wf.clear();
  wf.set_ocr(ocr);
  wf.set_gt(std::wregex{L"fa ta"}, L"fata");
  wf.set_gt(std::wregex{L"ſi unt"}, L"ſinunt");
  BOOST_CHECK_EQUAL(wf(), 2);
  wf.correct(*line, true); // correct partial
  BOOST_CHECK(not line->is_fully_corrected());

  auto words = line->words();
  BOOST_REQUIRE_EQUAL(words.size(), 5);
  BOOST_CHECK(not words[0].is_fully_corrected());
  BOOST_CHECK_EQUAL(words[0].cor(), "Dum");
  BOOST_CHECK(words[1].is_fully_corrected());
  BOOST_CHECK_EQUAL(words[1].cor(), "fata");
  BOOST_CHECK(words[2].is_fully_corrected());
  BOOST_CHECK_EQUAL(words[2].cor(), "ſinunt");
  BOOST_CHECK(not words[3].is_fully_corrected());
  BOOST_CHECK_EQUAL(words[3].cor(), "ivite");
  BOOST_CHECK(not words[4].is_fully_corrected());
  BOOST_CHECK_EQUAL(words[4].cor(), "laet");
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(WordBasedCorrection) {
  line = std::make_shared<Line>(1);
  line->append(ocr, 0, 100, 0.8);
  wf.set_ocr(*line);
  wf.set_gt("XYZ");
  const auto lev = wf(18, 5); // just correct `ivite` to `XYZ`
  BOOST_CHECK_EQUAL(lev, 5);
  wf.correct(*line, 18, 5);
  BOOST_CHECK(not line->is_fully_corrected());
  BOOST_CHECK(line->is_partially_corrected());
  const auto t = [](const auto &t, bool fc, bool pc, const auto &str) {
    BOOST_CHECK_EQUAL(t.is_fully_corrected(), fc);
    BOOST_CHECK_EQUAL(t.is_partially_corrected(), pc);
    BOOST_CHECK_EQUAL(t.cor(), str);
  };
  auto words = line->words();
  BOOST_REQUIRE_EQUAL(words.size(), 7);
  t(words[0], false, false, "Dum");
  t(words[1], false, false, "fa");
  t(words[2], false, false, "ta");
  t(words[3], false, false, "ſi");
  t(words[4], false, false, "unt");
  t(words[5], true, true, "XYZ");
  t(words[6], false, false, "laet");
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(Bug1) {
  wf.set_gt("De Homine.");
  wf.set_ocr("$=");
  BOOST_CHECK_EQUAL(wf(), 10);
  std::stringstream os;
  os << wf.trace();
  auto trace = os.str();
  BOOST_CHECK_EQUAL(trace, "--------##");
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(TokenWiseAtStart) {
  line = std::make_shared<Line>(1);
  line->append("ABCDE suffix", 0, 100, 0.8);
  wf.set_ocr(line->wcor());
  wf.set_gt("XYZXYZXYZ");
  BOOST_CHECK_EQUAL(wf(0, 5), 9);
  wf.correct(*line, 0, 9);
  BOOST_CHECK_EQUAL("XYZXYZXYZ suffix", line->cor());
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(TokenWiseMiddle) {
  line = std::make_shared<Line>(1);
  line->append("prefix ABCDE suffix", 0, 100, 0.8);
  wf.set_ocr(line->wcor());
  wf.set_gt("XYZXYZXYZ");
  BOOST_CHECK_EQUAL(wf(7, 5), 9);
  wf.correct(*line, 7, 9);
  BOOST_CHECK_EQUAL("prefix XYZXYZXYZ suffix", line->cor());
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(TokenWiseAtEnd) {
  line = std::make_shared<Line>(1);
  line->append("prefix ABCDE", 0, 100, 0.8);
  wf.set_ocr(line->wcor());
  wf.set_gt("XYZXYZXYZ");
  BOOST_CHECK_EQUAL(wf(7, 5), 9);
  wf.correct(*line, 7, 9);
  BOOST_CHECK_EQUAL("prefix XYZXYZXYZ", line->cor());
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_SUITE_END()
