#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE AltoXmlTestTest
#include <boost/test/unit_test.hpp>
#include <functional>
#include <iostream>
#include <vector>
#include "TmpDir.hpp"
#include "core/WagnerFischer.hpp"
#include "parser/ParserPage.hpp"
#include "parser/AltoXmlPageParser.hpp"
#include "parser/ParserChar.hpp"

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(ParsingTest)
{
	AltoXmlPageParser p("../misc/data/test/alto-test.xml");
	
	// exactly one Page!
	BOOST_REQUIRE(p.has_next());
	auto page = p.pparse();
	BOOST_CHECK(not p.has_next());
	
	// three lines
	BOOST_REQUIRE(page.lines.size() == 3);

	// <TextLine HEIGHT="90.0" WIDTH="3570.0" HPOS="3960.0" VPOS="972.0">
	BOOST_CHECK(page.lines[0].wstring() == L"ab cd ef");
	BOOST_CHECK(page.lines[0].box.left() == 3960);
	BOOST_CHECK(page.lines[0].box.top() == 972);
	BOOST_CHECK(page.lines[0].box.right() == 3960 + 3570);
	BOOST_CHECK(page.lines[0].box.bottom() == 972 + 90);
	BOOST_CHECK(page.lines[0].box.width() == 3570);
	BOOST_CHECK(page.lines[0].box.height() == 90);

	BOOST_CHECK(page.lines[1].wstring() == L"ab cd");
	BOOST_CHECK(page.lines[2].wstring() == L"abcd ef");
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(CorrectionTest)
{
	AltoXmlPageParser p("../misc/data/test/alto-test.xml");
	WagnerFischer wf;
	BOOST_REQUIRE(p.has_next());
	auto page = p.pparse();
	BOOST_REQUIRE(page.lines.size() == 3);

	// first line
	wf.set_truth(L"fe dc ba");
	wf.set_test(page.lines[0].wstring());
	auto lev = wf();
	BOOST_CHECK(lev == 6);
	wf.apply(page.lines[0]);
	BOOST_CHECK(page.lines[0].wstring() == L"fe dc ba");

	// second line (merge)
	wf.set_truth(L"abcd");
	wf.set_test(page.lines[1].wstring());
	lev = wf();
	BOOST_CHECK(lev == 1);
	wf.apply(page.lines[1]);
	BOOST_CHECK(page.lines[1].wstring() == L"abcd");

	// third line (split)
	wf.set_truth(L"ab cd ef");
	wf.set_test(page.lines[2].wstring());
	lev = wf();
	BOOST_CHECK(lev == 1);
	wf.apply(page.lines[2]);
	BOOST_CHECK(page.lines[2].wstring() == L"ab cd ef");

	// write and read;
	TmpDir tmp;
	auto file = tmp / "alto.xml";
	page.write(file);
}
