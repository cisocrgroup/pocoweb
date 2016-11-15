#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE OcropusLlocsTest

#include <boost/test/unit_test.hpp>
#include <functional>
#include <iostream>
#include <vector>
#include "TmpDir.hpp"
#include "core/WagnerFischer.hpp"
#include "parser/OcropusLlocsParserPage.hpp"
#include "parser/ParserPage.hpp"
#include "parser/XmlParserPage.hpp"
#include "parser/OcropusLlocsPageParser.hpp"

using namespace pcw;

struct Fixture {
	Fixture(): page() {
		OcropusLlocsPageParser parser("../misc/data/test/llocs-test/0001");
		BOOST_REQUIRE(parser.has_next());
		page = parser.pparse();
		BOOST_REQUIRE(not parser.has_next());
		BOOST_REQUIRE(page);
		BOOST_REQUIRE(page->size() == 3);
	}
	ParserPagePtr page;
};

////////////////////////////////////////////////////////////////////////////////
BOOST_FIXTURE_TEST_SUITE(OcropusLlocsTest, Fixture)

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(ParsingTest)
{
	Box box;
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
	wf.set_truth("fe dc ba");
	auto lev = page->get(0).correct(wf);
	BOOST_CHECK(lev == 6);
	BOOST_CHECK_EQUAL(page->get(0).string(), "fe dc ba");
	// std::cerr << wf << "\n";

	// second line (merge)
	wf.set_truth("abcd");
	lev = page->get(1).correct(wf);
	BOOST_CHECK(lev == 1);
	BOOST_CHECK_EQUAL(page->get(1).string(), "abcd");
	// std::cerr << wf << "\n";

	// third line (split)
	wf.set_truth("ab cd ef");
	lev = page->get(2).correct(wf);
	BOOST_CHECK(lev == 1);
	BOOST_CHECK_EQUAL(page->get(2).string(), "ab cd ef");
	// std::cerr << wf << "\n";

	// write and read;
	TmpDir tmp;
	page->write(tmp);

	OcropusLlocsPageParser p2(tmp / std::dynamic_pointer_cast<OcropusLlocsParserPage>(page)->dir());
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
