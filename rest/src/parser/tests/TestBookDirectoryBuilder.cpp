#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE BookDirectoryBuilderTest

#include <boost/test/unit_test.hpp>
#include <fstream>
#include <functional>
#include <iostream>
#include <vector>
#include "core/Book.hpp"
#include "core/BookDirectoryBuilder.hpp"
#include "core/Page.hpp"
#include "utils/TmpDir.hpp"

using namespace pcw;

struct Fixture {
	Fixture() : tmpdir(), builder(tmpdir.dir()) {}
	TmpDir tmpdir;
	BookDirectoryBuilder builder;
	void add_zip_file(const char* file) {
		std::ifstream is(file);
		is >> std::noskipws;
		BOOST_REQUIRE(is.good());
		std::string content;
		std::copy(std::istream_iterator<char>(is),
			  std::istream_iterator<char>(),
			  std::back_inserter(content));
		builder.add_zip_file_content(content);
	}
};

////////////////////////////////////////////////////////////////////////////////
BOOST_FIXTURE_TEST_SUITE(BookDirectoryBuilderTest, Fixture)

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(Ocropus) {
	add_zip_file("misc/data/test/hobbes-ocropus.zip");
	auto book = builder.build();
	BOOST_REQUIRE(book);
	BOOST_CHECK_EQUAL(book->size(), 2);

	// page 42
	auto p1 = book->find(42);
	BOOST_REQUIRE(p1);
	auto line = p1->find(0x27);
	BOOST_REQUIRE(line);
	BOOST_CHECK_EQUAL(line->ocr(),
			  "ſlanti Finis alicujus propoſiti. Contrà, "
			  "imaginatiotarda Defectum ani-");

	// page 100
	auto p2 = book->find(100);
	BOOST_REQUIRE(p2);
	line = p2->find(0x8);
	BOOST_REQUIRE(line);
	BOOST_CHECK_EQUAL(
	    line->ocr(),
	    "ea contribuere quæ ad Pacem & Deſenſionem ſuam neceſſaria ſunt,");

	// no such page
	auto p3 = book->find(200);
	BOOST_REQUIRE(not p3);
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_SUITE_END()
