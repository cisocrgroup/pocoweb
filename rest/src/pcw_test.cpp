#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE pcw_test
#include <boost/test/unit_test.hpp>
#include <functional>
#include <iostream>
#include <vector>
#include "parser/AltoXmlPageParser.hpp"
#include "parser/ParserChar.hpp"

using namespace pcw;

// <TextLine HEIGHT="90.0" WIDTH="3570.0" HPOS="3960.0" VPOS="972.0">
////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(alto_xml_parser)
{
	AltoXmlPageParser p("misc/data/test/alto-test.xml");
	
	// exactly one Page!
	BOOST_REQUIRE(p.has_next());
	auto page = p.pparse();
	BOOST_CHECK(not p.has_next());
	
	// three lines
	BOOST_REQUIRE(page.lines.size() == 3);
	BOOST_CHECK(page.lines[0].string() == L"ab cd ef");

	BOOST_CHECK(page.lines[0].box.left() == 3960);
	BOOST_CHECK(page.lines[0].box.right() == 972);
	BOOST_CHECK(page.lines[0].box.width() == 3570);
	BOOST_CHECK(page.lines[0].box.height() == 90);

	BOOST_CHECK(page.lines[1].string() == L"ab cd");
	BOOST_CHECK(page.lines[2].string() == L"abcd ef");
}

