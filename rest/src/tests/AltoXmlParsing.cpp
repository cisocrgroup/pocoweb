#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE AltoXmlTest

#include <boost/test/unit_test.hpp>
#include <functional>
#include <iostream>
#include <vector>
#include "TmpDir.hpp"
#include "core/WagnerFischer.hpp"
#include "parser/ParserPage.hpp"
#include "parser/XmlParserPage.hpp"
#include "parser/AltoXmlPageParser.hpp"

using namespace pcw;

struct Fixture {
	Fixture(): page() {
		AltoXmlPageParser parser("../misc/data/test/alto-test.xml");
		BOOST_REQUIRE(parser.has_next());
		page = parser.pparse();
		BOOST_REQUIRE(not parser.has_next());
		BOOST_REQUIRE(page);
		BOOST_REQUIRE(page->size() == 3);
	}
	ParserPagePtr page;
};

////////////////////////////////////////////////////////////////////////////////
BOOST_FIXTURE_TEST_SUITE(AltoXmlTest, Fixture)

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(ParsingTest)
{
	// <TextLine HEIGHT="90.0" WIDTH="3570.0" HPOS="3960.0" VPOS="972.0">
	// check_l
	Box box{3960, 972, 3960 + 3570, 972 + 90};
	BOOST_CHECK_EQUAL(page->get(0).string(), "ab cd ef");
	BOOST_CHECK_EQUAL(page->get(0).box, box);
	BOOST_CHECK_EQUAL(page->get(1).string(), "ab cd");
	BOOST_CHECK_EQUAL(page->get(1).box, box);
	BOOST_CHECK_EQUAL(page->get(2).string(), "abcd ef");
	BOOST_CHECK_EQUAL(page->get(2).box, box);
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(CorrectionTest)
{
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
	auto file = tmp / "alto.xml";
	page->write(file);

	AltoXmlPageParser p2(file);
	page = p2.pparse();
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
