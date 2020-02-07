#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE CalamariTest

#include "core/Line.hpp"
#include "parser/CalamariParserLine.hpp"
#include <boost/test/unit_test.hpp>

using namespace pcw;

const Path proto = "misc/data/test/00123.pred";
const Path img = "misc/data/test/00123.nrm.png";

struct LineFixture {
  LineFixture() : line(proto, img) {}
  CalamariParserLine line;
};

BOOST_FIXTURE_TEST_SUITE(CalamariTest, LineFixture)

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(CheckReadLineString) {
  BOOST_CHECK_EQUAL(line.string(),
                    "Bahn des Ruhms, die dem Helden und dem Staats—");
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(CheckBuildLineId) {
  BOOST_REQUIRE(line.line(123) != nullptr);
  BOOST_CHECK_EQUAL(line.line(123)->id(), 123);
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(CheckBuildLineImg) {
  BOOST_REQUIRE(line.line(123) != nullptr);
  BOOST_CHECK_EQUAL(line.line(123)->img, img);
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(CheckBuildLineString) {
  BOOST_REQUIRE(line.line(123) != nullptr);
  BOOST_CHECK_EQUAL(line.line(123)->cor(),
                    "Bahn des Ruhms, die dem Helden und dem Staats—");
}

BOOST_AUTO_TEST_SUITE_END()
