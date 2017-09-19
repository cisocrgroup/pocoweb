#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE CorrectorTest

#include <boost/test/unit_test.hpp>
#include <functional>
#include <iostream>
#include <vector>
#include "core/Book.hpp"
#include "core/Corrector.hpp"
#include "core/Line.hpp"
#include "core/Page.hpp"

using namespace pcw;

struct Fixture {
	static std::vector<std::string> p1;
	static std::vector<std::string> p2;
	Fixture() : book(std::make_shared<Book>()), corrector() {
		auto page1 = std::make_shared<Page>(1);
		for (const auto& l : p1) {
			auto line = std::make_shared<Line>(0);
			line->append(l, 0, 100, .8);
			page1->push_back(std::move(line));
		}
		book->push_back(*page1);
		auto page2 = std::make_shared<Page>(2);
		for (const auto& l : p2) {
			auto line = std::make_shared<Line>(0);
			line->append(l, 0, 100, .8);
			page2->push_back(std::move(line));
		}
		book->push_back(*page2);
		corrector.set_view(book);
		BOOST_REQUIRE_EQUAL(book->size(), 2);
		BOOST_REQUIRE(book->find(1));
		BOOST_REQUIRE_EQUAL(book->find(1)->size(), 3);
		BOOST_REQUIRE(book->find(2));
		BOOST_REQUIRE_EQUAL(book->find(2)->size(), 3);
	}
	BookPtr book;
	Corrector corrector;
};

////////////////////////////////////////////////////////////////////////////////
std::vector<std::string> Fixture::p1 = {
    "Gallia eft omnis divifa in partef tref,",
    "quarum unam incolunt Belgae, aliam Aquitani,",
    "tertiam qui ipforum lingua Celtae, noftra Galli appellantur.",
};

////////////////////////////////////////////////////////////////////////////////
std::vector<std::string> Fixture::p2 = {
    "Hi omnes lingua, institutif, legibuf inter fe differunt.",
    "Gallof ab Aquitanif Garumna flumen, a Belgif Matrona et Sequana dividit.",
    "Horum omnium fortiffimi funt Belgae...",
};

////////////////////////////////////////////////////////////////////////////////
BOOST_FIXTURE_TEST_SUITE(CorrectorTest, Fixture)

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(LineWise) {
	corrector.correct(1, 1, "f", "ſ");
	BOOST_CHECK_EQUAL((*book->find(1))[1].cor(),
			  "Gallia eſt omnis diviſa in parteſ treſ,");
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(PageWise) {
	corrector.correct(1, "f", "ſ");
	BOOST_CHECK_EQUAL((*book->find(1))[1].cor(),
			  "Gallia eſt omnis diviſa in parteſ treſ,");
	BOOST_CHECK_EQUAL((*book->find(1))[2].cor(),
			  "quarum unam incolunt Belgae, aliam Aquitani,");
	BOOST_CHECK_EQUAL(
	    (*book->find(1))[3].cor(),
	    "tertiam qui ipſorum lingua Celtae, noſtra Galli appellantur.");
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(BookWise) {
	corrector.correct("f", "ſ");
	BOOST_CHECK_EQUAL((*book->find(1))[1].cor(),
			  "Gallia eſt omnis diviſa in parteſ treſ,");
	BOOST_CHECK_EQUAL((*book->find(1))[2].cor(),
			  "quarum unam incolunt Belgae, aliam Aquitani,");
	BOOST_CHECK_EQUAL(
	    (*book->find(1))[3].cor(),
	    "tertiam qui ipſorum lingua Celtae, noſtra Galli appellantur.");
	BOOST_CHECK_EQUAL(
	    (*book->find(2))[1].cor(),
	    "Hi omnes lingua, institutiſ, legibuſ inter ſe diſſerunt.");
	BOOST_CHECK_EQUAL((*book->find(2))[2].cor(),
			  "Galloſ ab Aquitaniſ Garumna ſlumen, a Belgiſ "
			  "Matrona et Sequana dividit.");
	BOOST_CHECK_EQUAL((*book->find(2))[3].cor(),
			  "Horum omnium ſortiſſimi ſunt Belgae...");
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_SUITE_END()
