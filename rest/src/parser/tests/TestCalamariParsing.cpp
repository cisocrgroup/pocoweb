#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE CalamariTest

#include "parser/CalamariParserLine.hpp"
#include <boost/test/unit_test.hpp>

using namespace pcw;

struct LineFixture {
  LineFixture() : line("misc/data/test/00123.pred") {}
  CalamariParserLine line;
};

BOOST_FIXTURE_TEST_SUITE(CalamariTest, LineFixture)

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(CheckReadLineString) {
  BOOST_CHECK_EQUAL(line.string(),
                    "Bahn des Ruhms, die dem Helden und dem Staats—");
}

BOOST_AUTO_TEST_SUITE_END()
