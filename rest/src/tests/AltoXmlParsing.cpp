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
	BOOST_CHECK(page->get(0).wstring() == L"ab cd ef");
	BOOST_CHECK(page->get(0).box.left() == 3960);
	BOOST_CHECK(page->get(0).box.top() == 972);
	BOOST_CHECK(page->get(0).box.right() == 3960 + 3570);
	BOOST_CHECK(page->get(0).box.bottom() == 972 + 90);
	BOOST_CHECK(page->get(0).box.width() == 3570);
	BOOST_CHECK(page->get(0).box.height() == 90);

	BOOST_CHECK(page->get(1).wstring() == L"ab cd");
	BOOST_CHECK(page->get(2).wstring() == L"abcd ef");
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(CorrectionTest)
{
	WagnerFischer wf;

	// first line
	wf.set_truth(L"fe dc ba");
	auto lev = page->get(0).correct(wf);
	BOOST_CHECK(lev == 6);
	BOOST_CHECK(page->get(0).wstring() == L"fe dc ba");
	// std::cerr << wf << "\n";

	// second line (merge)
	wf.set_truth(L"abcd");
	lev = page->get(1).correct(wf);
	BOOST_CHECK(lev == 1);
	BOOST_CHECK(page->get(1).wstring() == L"abcd");
	// std::cerr << wf << "\n";

	// third line (split)
	wf.set_truth(L"ab cd ef");
	lev = page->get(2).correct(wf);
	BOOST_CHECK(lev == 1);
	BOOST_CHECK(page->get(2).wstring() == L"ab cd ef");
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
	BOOST_CHECK(page->get(0).wstring() == L"fe dc ba");
	BOOST_CHECK(page->get(1).wstring() == L"abcd");
	BOOST_CHECK(page->get(2).wstring() == L"ab cd ef");
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_SUITE_END()
