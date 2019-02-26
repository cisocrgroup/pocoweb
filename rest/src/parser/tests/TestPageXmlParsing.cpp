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

struct Fixture
{
  Fixture()
    : page()
  {
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
BOOST_AUTO_TEST_CASE(PageXmlParserParsesPageBox)
{
  Box box{ 0, 0, 1756, 2440 };
  BOOST_CHECK_EQUAL(box, page->box);
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(PageXmlParserParsesImageFileName)
{
  BOOST_CHECK_EQUAL("path/to/img.png", page->img);
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(PageXmlParserParsesTwoLines)
{
  BOOST_CHECK_EQUAL(2, page->size());
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(PageXmlParserCheckFirstLine)
{
  BOOST_CHECK_EQUAL(std::string("x y"), page->get(0).string());
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(PageXmlParserCheckSecondLine)
{
  BOOST_CHECK_EQUAL(std::string("ab cd"), page->get(1).string());
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(PageXmlParserCheckFirstLineBox)
{
  BOOST_CHECK_EQUAL(Box(1, 2, 11, 12), page->get(0).line(1)->box);
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_SUITE_END()
