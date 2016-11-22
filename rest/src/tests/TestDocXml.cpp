#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE DocXmlTest

#include <boost/test/unit_test.hpp>
#include <functional>
#include <iostream>
#include <vector>
#include "core/docxml.hpp"
#include "core/Book.hpp"
#include "core/Page.hpp"
#include "core/Line.hpp"

using namespace pcw;

struct Fixture {
	static std::vector<std::string> p1;
	static std::vector<std::string> p2;
	Fixture(): docxml(), root() {
		auto page1 = std::make_shared<Page>(1);
		page1->img = "test1.img";
		page1->ocr = "test1.ocr";
		page1->push_back(line("Он привыно, без хлопот"));
		page1->push_back(line("снова ждет,"));

		auto page2 = std::make_shared<Page>(2);
		page2->img = "test2.img";
		page2->ocr = "test2.ocr";
		page2->push_back(line("Что галушка попадет"));
		page2->push_back(line("прямо в рот..."));

		auto book = std::make_shared<Book>();
		book->push_back(page1);
		book->push_back(page2);

		BOOST_REQUIRE_EQUAL(book->size(), 2);
		BOOST_REQUIRE(book->find(1));
		BOOST_REQUIRE_EQUAL(book->find(1)->size(), 2);
		BOOST_REQUIRE(book->find(2));
		BOOST_REQUIRE_EQUAL(book->find(2)->size(), 2);

		DocXml os(docxml);
		os << *book;
		root = docxml.document_element();
	}
	static LinePtr line(const char* lstr) {
		auto line = std::make_shared<Line>(0);
		if (lstr)
			line->append(lstr, 0, 100, .8);
		return line;
	}
	pugi::xml_document docxml;
	pugi::xml_node root;
};

////////////////////////////////////////////////////////////////////////////////
BOOST_FIXTURE_TEST_SUITE(DocXmlTest, Fixture)

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(DocumentPages)
{
	BOOST_CHECK_EQUAL(root.name(), "document");
	BOOST_CHECK_EQUAL(root.first_child().name(), "page");
	BOOST_CHECK_EQUAL(root.first_child().next_sibling().name(), "page");
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(Page)
{
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_SUITE_END()
