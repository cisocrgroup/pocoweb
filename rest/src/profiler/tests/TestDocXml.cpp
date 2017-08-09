#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE DocXmlTest

#include <boost/test/unit_test.hpp>
#include <fstream>
#include <functional>
#include <iostream>
#include <vector>
#include "core/Book.hpp"
#include "core/Line.hpp"
#include "core/Page.hpp"
#include "profiler/docxml.hpp"
#include "utils/TmpDir.hpp"

using namespace pcw;

struct Fixture {
	static std::vector<std::string> p1;
	static std::vector<std::string> p2;
	Fixture() : docxml(), root() {
		auto page1 = std::make_shared<Page>(1);
		page1->img = "test1.img";
		page1->ocr = "test1.ocr";
		page1->push_back(line("Он привыно, без хлопот"));
		page1->push_back(line("снова ждет,"));

		auto page2 = std::make_shared<Page>(2, Box{0, 10, 100, 20});
		page2->img = "test2.img";
		page2->ocr = "test2.ocr";
		page2->push_back(line("Что галушка попадет"));
		page2->push_back(line("прямо в рот..."));

		auto book = std::make_shared<Book>();
		book->push_back(*page1);
		book->push_back(*page2);

		BOOST_REQUIRE_EQUAL(book->size(), 2);
		BOOST_REQUIRE(book->find(1));
		BOOST_REQUIRE_EQUAL(book->find(1)->size(), 2);
		BOOST_REQUIRE(book->find(2));
		BOOST_REQUIRE_EQUAL(book->find(2)->size(), 2);

		docxml << *book;
		root = docxml.document_element();
	}
	static LinePtr line(const char* lstr) {
		// all lines get the same bounding boxes
		auto line = std::make_shared<Line>(0, Box{0, 0, 100, 10});
		if (lstr) line->append(lstr, 0, 100, .8);
		return line;
	}
	DocXml docxml;
	pugi::xml_node root;
};

////////////////////////////////////////////////////////////////////////////////
BOOST_FIXTURE_TEST_SUITE(DocXmlTest, Fixture)

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(Document) { BOOST_CHECK_EQUAL(root.name(), "document"); }

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(Page1) {
	// docxml.save(std::cerr);
	auto page = root.first_child();
	BOOST_CHECK_EQUAL(page.name(), "page");
	BOOST_CHECK_EQUAL(page.attribute("imageFile").value(), "test1.img");
	BOOST_CHECK_EQUAL(page.attribute("sourceFile").value(), "test1.ocr");

	auto b = pugi::xml_node_iterator(page.first_child());
	BOOST_CHECK_EQUAL(std::next(b, 0)->name(), "token");
	BOOST_CHECK_EQUAL(std::next(b, 0)->child("wOCR").child_value(), "Он");
	BOOST_CHECK_EQUAL(std::next(b, 1)->name(), "token");
	BOOST_CHECK_EQUAL(std::next(b, 1)->child("wOCR").child_value(),
			  "привыно");
	BOOST_CHECK_EQUAL(std::next(b, 2)->name(), "token");
	BOOST_CHECK_EQUAL(std::next(b, 2)->child("wOCR").child_value(), "без");
	BOOST_CHECK_EQUAL(std::next(b, 3)->name(), "token");
	BOOST_CHECK_EQUAL(std::next(b, 3)->child("wOCR").child_value(),
			  "хлопот");
	BOOST_CHECK_EQUAL(std::next(b, 4)->name(), "token");
	BOOST_CHECK_EQUAL(std::next(b, 4)->child("wOCR").child_value(),
			  "снова");
	BOOST_CHECK_EQUAL(std::next(b, 5)->name(), "token");
	BOOST_CHECK_EQUAL(std::next(b, 5)->child("wOCR").child_value(), "ждет");
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(Page2) {
	auto page = root.first_child().next_sibling();
	BOOST_CHECK_EQUAL(page.name(), "page");
	BOOST_CHECK_EQUAL(page.attribute("imageFile").value(), "test2.img");
	BOOST_CHECK_EQUAL(page.attribute("sourceFile").value(), "test2.ocr");

	auto b = pugi::xml_node_iterator(page.first_child());
	BOOST_CHECK_EQUAL(std::next(b, 0)->name(), "token");
	BOOST_CHECK_EQUAL(std::next(b, 0)->child("wOCR").child_value(), "Что");
	BOOST_CHECK_EQUAL(std::next(b, 1)->name(), "token");
	BOOST_CHECK_EQUAL(std::next(b, 1)->child("wOCR").child_value(),
			  "галушка");
	BOOST_CHECK_EQUAL(std::next(b, 2)->name(), "token");
	BOOST_CHECK_EQUAL(std::next(b, 2)->child("wOCR").child_value(),
			  "попадет");
	BOOST_CHECK_EQUAL(std::next(b, 3)->name(), "token");
	BOOST_CHECK_EQUAL(std::next(b, 3)->child("wOCR").child_value(),
			  "прямо");
	BOOST_CHECK_EQUAL(std::next(b, 4)->name(), "token");
	BOOST_CHECK_EQUAL(std::next(b, 4)->child("wOCR").child_value(), "в");
	BOOST_CHECK_EQUAL(std::next(b, 5)->name(), "token");
	BOOST_CHECK_EQUAL(std::next(b, 5)->child("wOCR").child_value(), "рот");
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(BoundingBoxes) {
	for (const auto& page : root.children("page")) {
		for (const auto& token : page.children("token")) {
			auto coord = token.child("coord");
			BOOST_CHECK_EQUAL(coord.attribute("t").as_int(), 0);
			BOOST_CHECK_EQUAL(coord.attribute("b").as_int(), 10);
			BOOST_CHECK(coord.attribute("l").as_int() >= 0);
			BOOST_CHECK(coord.attribute("r").as_int() > 0);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(DocXmlIO) {
	TmpDir tmp;
	auto file = tmp / "test.docxml";

	// write
	std::ofstream os(file.string());
	BOOST_REQUIRE(os.good());
	docxml.save(os);
	os.close();

	// read
	std::ifstream is(file.string());
	BOOST_REQUIRE(is.good());
	pugi::xml_document doc;
	auto ok = doc.load(is);
	BOOST_CHECK(ok);
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_SUITE_END()
