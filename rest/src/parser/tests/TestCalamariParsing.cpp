#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE CalamariTest

#include "core/Line.hpp"
#include "core/WagnerFischer.hpp"
#include "utils/TmpDir.hpp"
#include "parser/CalamariPage.hpp"
#include "parser/CalamariParserLine.hpp"
#include <boost/test/unit_test.hpp>
#include <boost/filesystem/operations.hpp>

namespace fs = boost::filesystem;
using namespace pcw;

const Path dir = "misc/data/test/calamari/0013";
const Path proto = dir / "00123.pred";
const Path img = dir / "00123.nrm.png";

struct LineFixture {
  LineFixture() : line(proto, img) {}
  CalamariParserLine line;
};

BOOST_FIXTURE_TEST_SUITE(CalamariLineTest, LineFixture)

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

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(CheckBuildLineBox) {
  BOOST_REQUIRE(line.line(123) != nullptr);
  BOOST_CHECK_EQUAL(line.line(123)->box, (Box{0, 0, 952, 43}));
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(CheckBuildSetChar) {
  WagnerFischer wf;
  std::string gt("bahn des ruhms, die dem helden und dem staats—");
  wf.set_gt(gt);
  const auto lev = line.correct(wf);
  BOOST_CHECK_EQUAL(lev, 4);
  BOOST_CHECK_EQUAL(line.string(), gt);
  BOOST_CHECK_EQUAL(line.line(123)->cor(), gt);
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(CheckBuildEraseChar) {
  WagnerFischer wf;
  std::string gt("BahndesRuhms,diedemHeldenunddemStaats—");
  wf.set_gt(gt);
  const auto lev = line.correct(wf);
  BOOST_CHECK_EQUAL(lev, 8);
  BOOST_CHECK_EQUAL(line.string(), gt);
  BOOST_CHECK_EQUAL(line.line(123)->cor(), gt);
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(CheckBuildInsertChar) {
  WagnerFischer wf;
  std::string gt("X Bahn X des X Ruhms, X die X dem X Helden und dem Staats—");
  wf.set_gt(gt);
  const auto lev = line.correct(wf);
  BOOST_CHECK_EQUAL(lev, 12);
  BOOST_CHECK_EQUAL(line.string(), gt);
  BOOST_CHECK_EQUAL(line.line(123)->cor(), gt);
}

BOOST_AUTO_TEST_SUITE_END()

struct PageFixture {
  PageFixture() : page(dir) {}
  CalamariPageParser page;
};

BOOST_FIXTURE_TEST_SUITE(CalamariPageTest, PageFixture)

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(CheckParseDirSize) {
  BOOST_REQUIRE(page.parse() != nullptr);
  BOOST_CHECK_EQUAL(page.parse()->size(), 1);
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(CheckParsedLineString) {
  BOOST_REQUIRE(page.parse() != nullptr);
  BOOST_CHECK_EQUAL(page.parse()->get(0).string(),
                    "Bahn des Ruhms, die dem Helden und dem Staats—");
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(CheckWritePage) {
  TmpDir tmp;
  BOOST_REQUIRE(page.parse() != nullptr);
  page.parse()->write(tmp);
  BOOST_CHECK_EQUAL(fs::is_regular_file(tmp / proto.filename()), true);
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(CheckWritePageLine) {
  TmpDir tmp;
  BOOST_REQUIRE(page.parse() != nullptr);
  page.parse()->write(tmp);
  CalamariParserLine line(tmp / proto.filename(), "");
  BOOST_CHECK_EQUAL(line.string(),
                    "Bahn des Ruhms, die dem Helden und dem Staats—");
}

BOOST_AUTO_TEST_SUITE_END()
