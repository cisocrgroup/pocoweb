#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE PageXmlTest

#include "core/Line.hpp"
#include "core/WagnerFischer.hpp"
#include "parser/PageXmlPageParser.hpp"
#include "parser/ParserPage.hpp"
#include "parser/XmlParserPage.hpp"
#include "utils/TmpDir.hpp"
#include <boost/test/unit_test.hpp>
#include <functional>
#include <iostream>
#include <vector>

using namespace pcw;

struct Fixture {
  Fixture() : page() {
    PageXmlPageParser parser("misc/data/test/page-test.xml");
    BOOST_REQUIRE(parser.has_next());
    page = parser.parse();
    BOOST_REQUIRE(not parser.has_next());
    BOOST_REQUIRE(page);
  }
  ParserPagePtr page;
};

////////////////////////////////////////////////////////////////////////////////
BOOST_FIXTURE_TEST_SUITE(PageXmlTest, Fixture)

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(PageXmlParserParsesPageBox) {
  Box box{0, 0, 1756, 2440};
  BOOST_CHECK_EQUAL(box, page->box);
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(PageXmlParserParsesImageFileName) {
  BOOST_CHECK_EQUAL("path/to/img.png", page->img);
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(PageXmlParserParsesTwoLines) {
  BOOST_CHECK_EQUAL(2, page->size());
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(PageXmlParserCheckFirstLine) {
  BOOST_CHECK_EQUAL(std::string("x y"), page->get(0).string());
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(PageXmlParserCheckSecondLine) {
  BOOST_CHECK_EQUAL(std::string("ab cd"), page->get(1).string());
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(PageXmlParserCheckFirstLineBox) {
  BOOST_CHECK_EQUAL(Box(1, 2, 11, 12), page->get(0).line(1)->box);
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(PageXmlParserCheckFirstLineParsedString) {
  BOOST_CHECK_EQUAL("x y", page->get(0).line(1)->ocr());
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_SUITE_END()

struct WFFixture : Fixture {
  WFFixture() : Fixture(), wf() {}
  WagnerFischer wf;
};

////////////////////////////////////////////////////////////////////////////////
BOOST_FIXTURE_TEST_SUITE(PageXmlCorrectionTest, WFFixture)

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(PageXmlParserCheckCorrectLine) {
  wf.set_gt("a b");
  page->get(0).correct(wf);
  BOOST_CHECK_EQUAL(std::string("a b"), page->get(0).string());
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(PageXmlParserCheckCorrectLineSmaller) {
  wf.set_gt("ab");
  page->get(1).correct(wf);
  BOOST_CHECK_EQUAL(std::string("ab"), page->get(1).string());
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(PageXmlParserCheckCorrectLineLarger) {
  wf.set_gt("ab c ef");
  page->get(1).correct(wf);
  BOOST_CHECK_EQUAL(std::string("ab c ef"), page->get(1).string());
  Path p("/tmp/out.xml");
  page->write(p);
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_SUITE_END()

struct HerrnhutereyFixture {
  HerrnhutereyFixture() : page() {
    PageXmlPageParser parser("misc/data/test/herrnhuterey-page.xml");
    BOOST_REQUIRE(parser.has_next());
    page = parser.parse();
    BOOST_REQUIRE(not parser.has_next());
    BOOST_REQUIRE(page);
  }
  ParserPagePtr page;
};

////////////////////////////////////////////////////////////////////////////////
BOOST_FIXTURE_TEST_SUITE(HerrnhuteryPageXmlTest, HerrnhutereyFixture)

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(HerrnhutereyCheckImgPath) {
  BOOST_CHECK_EQUAL("benner_herrnhuterey04_1748_0013.tif", page->img);
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(HerrnhutereyCheckFirstLine) {
  BOOST_CHECK_EQUAL(std::string("1"), page->get(0).string());
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(HerrnhutereyCheckSecondLine) {
  BOOST_CHECK_EQUAL(std::string("Die"), page->get(1).string());
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(HerrnhutereyCheckThirdLine) {
  BOOST_CHECK_EQUAL(std::string("Herrnhuterey"), page->get(2).string());
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(HerrnhutereyCheckFourthLine) {
  BOOST_CHECK_EQUAL(std::string("in ihrer Schalkheit."), page->get(3).string());
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_SUITE_END()
