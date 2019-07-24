#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE OcropusLlocsTest

#include "core/Line.hpp"
#include "core/WagnerFischer.hpp"
#include "core/util.hpp"
#include "parser/OcropusLlocsPageParser.hpp"
#include "parser/OcropusLlocsParserPage.hpp"
#include "parser/ParserPage.hpp"
#include "parser/XmlParserPage.hpp"
#include "utils/TmpDir.hpp"
#include <boost/test/unit_test.hpp>
#include <fstream>
#include <functional>
#include <iostream>
#include <vector>

using namespace pcw;

struct Fixture {
  Fixture() : page() {
    OcropusLlocsPageParser parser("misc/data/test/llocs-test/0001");
    BOOST_REQUIRE(parser.has_next());
    page = parser.parse();
    BOOST_REQUIRE(not parser.has_next());
    BOOST_REQUIRE(page);
    BOOST_REQUIRE(page->size() == 3);
  }
  ParserPagePtr page;
};

////////////////////////////////////////////////////////////////////////////////
BOOST_FIXTURE_TEST_SUITE(OcropusLlocsTest, Fixture)

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(ParsingTest) {
  Box box;
  BOOST_CHECK_EQUAL(page->get(0).string(), "ab cd ef");
  BOOST_CHECK_EQUAL(page->get(0).box, box);
  BOOST_CHECK_EQUAL(page->get(1).string(), "ab cd");
  BOOST_CHECK_EQUAL(page->get(1).box, box);
  BOOST_CHECK_EQUAL(page->get(2).string(), "abcd ef");
  BOOST_CHECK_EQUAL(page->get(2).box, box);
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
  page->write(tmp / "1");

  // page->write only writes the llocs files.
  // the parser needs the png files. So copy them.
  for (auto i = 0U; i < 3; ++i) {
    const auto &line = page->get(i);
    const auto to = tmp / "1" / line.line(i)->img.filename();
    hard_link_or_copy(line.line(i)->img, to);
  }

  OcropusLlocsPageParser p2(tmp / "1");
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
