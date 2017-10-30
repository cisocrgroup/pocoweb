#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE BookDirectoryBuilderTest

#include <crow/logging.h>
#include <boost/filesystem/operations.hpp>
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

struct ZipFile {
	public:
		ZipFile(const std::string& file): file_(file) {
			system(("wget http://www.cis.lmu.de/~finkf/pocoweb/" + file_).data());
		}
		~ZipFile() {
			boost::system::error_code ec;
			boost::filesystem::remove(file_, ec);
		}
		const std::string& file() {return file_;}
	private:
		const std::string file_;
};

struct Fixture {
	Fixture() : tmpdir(), dir("testdir"), builder(tmpdir.dir(), dir) {
		// static crow::CerrLogHandler cerrlogger;
		// crow::logger::setHandler(&cerrlogger);
		// crow::logger::setLogLevel(crow::LogLevel::Debug);
	}
	TmpDir tmpdir;
	Path dir;
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
	ZipFile zip("hobbes-ocropus.zip");
	add_zip_file(zip.file().data());
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
	BOOST_CHECK_EQUAL(line->img,
			  dir / "ocropus-book" / "0042" / "010027.bin.png");

	// page 100
	auto p2 = book->find(100);
	BOOST_REQUIRE(p2);
	line = p2->find(0x8);
	BOOST_REQUIRE(line);
	BOOST_CHECK_EQUAL(
	    line->ocr(),
	    "ea contribuere quæ ad Pacem & Deſenſionem ſuam neceſſaria ſunt,");
	BOOST_CHECK_EQUAL(line->img,
			  dir / "ocropus-book" / "0100" / "010008.bin.png");

	// no such page
	auto p3 = book->find(200);
	BOOST_REQUIRE(not p3);
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(Alto) {
	ZipFile zip("rollenhagen_reysen_1603.zip");
	add_zip_file(zip.file().data());
	auto book = builder.build();
	BOOST_REQUIRE(book);
	BOOST_CHECK_EQUAL(book->size(), 3);

	// first page
	const auto p1 = book->find(1);
	BOOST_REQUIRE(p1);
	BOOST_CHECK_EQUAL(p1->img,
			  dir /
			      "rollenhagen_reysen_1603/rollenhagen_reysen_1603/"
			      "rollenhagen_reysen_1603_0007.tif");
	auto line = p1->find(1);
	BOOST_REQUIRE(line);
	BOOST_CHECK_EQUAL(line->ocr(), "Vnd daſſelb im Bluthroten Felde/");
	BOOST_CHECK_EQUAL(
	    line->img, dir / "line-images" / "0000000001" / "0000000001.png");

	// second page
	const auto p2 = book->find(2);
	BOOST_REQUIRE(p2);
	line = p2->find(6);
	BOOST_REQUIRE(line);
	BOOST_CHECK_EQUAL(line->ocr(), "vnd des Jndianiſchen Landes/ auch von");
	BOOST_CHECK_EQUAL(
	    line->img, dir / "line-images" / "0000000002" / "0000000006.png");

	// third page
	const auto p3 = book->find(3);
	BOOST_REQUIRE(p2);
	line = p3->find(14);
	BOOST_REQUIRE(line);
	BOOST_CHECK_EQUAL(
	    line->ocr(), "in je einem Land ſo viel ſeltzame wunder wehrẽ/ wen");
	BOOST_CHECK_EQUAL(
	    line->img, dir / "line-images" / "0000000003" / "000000000e.png");

	// no such page
	const auto p4 = book->find(4);
	BOOST_REQUIRE(not p4);
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_SUITE_END()
