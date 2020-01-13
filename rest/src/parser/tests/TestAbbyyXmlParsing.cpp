#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE AbbyyXmlTest

#include "core/Line.hpp"
#include "core/WagnerFischer.hpp"
#include "parser/AbbyyXmlPageParser.hpp"
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
    AbbyyXmlPageParser parser("misc/data/test/abbyy-test.xml");
    BOOST_REQUIRE(parser.has_next());
    page = parser.parse();
    BOOST_REQUIRE(not parser.has_next());
    BOOST_REQUIRE(page);
    BOOST_REQUIRE(page->size() == 3);
  }
  ParserPagePtr page;
};

////////////////////////////////////////////////////////////////////////////////
BOOST_FIXTURE_TEST_SUITE(AbbyyXmlTest, Fixture)

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(ParsingTest) {
  Box box{258, 126, 1958, 294};
  BOOST_CHECK_EQUAL(page->get(0).string(), "ab cd ef");
  BOOST_CHECK_EQUAL(page->get(0).box, box);
  BOOST_CHECK_EQUAL(page->get(1).string(), "ab cd");
  BOOST_CHECK_EQUAL(page->get(1).box, box);
  BOOST_CHECK_EQUAL(page->get(2).string(), "abcd ef");
  BOOST_CHECK_EQUAL(page->get(2).box, box);
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(BuildLinesTest) {
  Box box{258, 126, 1958, 294};
  BOOST_CHECK_EQUAL(page->get(0).line(0)->cor(), "ab cd ef");
  BOOST_CHECK_EQUAL(page->get(0).line(0)->box, box);
  BOOST_CHECK_EQUAL(page->get(1).line(1)->cor(), "ab cd");
  BOOST_CHECK_EQUAL(page->get(1).line(1)->box, box);
  BOOST_CHECK_EQUAL(page->get(2).line(2)->cor(), "abcd ef");
  BOOST_CHECK_EQUAL(page->get(2).line(2)->box, box);
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(CorrectionTest) {
  WagnerFischer wf;

  // first line
  wf.set_gt("fe dc ba");
  auto lev = page->get(0).correct(wf);
  BOOST_CHECK(lev == 6);
  BOOST_CHECK_EQUAL(page->get(0).string(), "fe dc ba");
  // std::cerr << wf << "\n";

  // second line (merge)
  wf.set_gt("abcd");
  lev = page->get(1).correct(wf);
  BOOST_CHECK(lev == 1);
  BOOST_CHECK_EQUAL(page->get(1).string(), "abcd");
  // std::cerr << wf << "\n";

  // third line (split)
  wf.set_gt("ab cd ef");
  lev = page->get(2).correct(wf);
  BOOST_CHECK(lev == 1);
  BOOST_CHECK_EQUAL(page->get(2).string(), "ab cd ef");
  // std::cerr << wf << "\n";

  // write and read;
  TmpDir tmp;
  auto file = tmp / "abbyy.xml";
  page->write(file);

  AbbyyXmlPageParser p2(file);
  page = p2.parse();
  BOOST_REQUIRE(page != nullptr);
  BOOST_REQUIRE(page->size() == 3);

  // std::cerr << "0: " << page->get(0).string() << "\n";
  // std::cerr << "1: " << page->get(1).string() << "\n";
  // std::cerr << "2: " << page->get(2).string() << "\n";
  BOOST_CHECK_EQUAL(page->get(0).string(), "fe dc ba");
  BOOST_CHECK_EQUAL(page->get(1).string(), "abcd");
  BOOST_CHECK_EQUAL(page->get(2).string(), "ab cd ef");
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_SUITE_END()
