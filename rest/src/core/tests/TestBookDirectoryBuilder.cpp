#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE BookDirectoryBuilderTest

#include "core/Book.hpp"
#include "core/BookDirectoryBuilder.hpp"
#include "core/Page.hpp"
#include "core/PageBuilder.hpp"
#include "utils/TmpDir.hpp"
#include <boost/test/unit_test.hpp>
#include <iostream>

using namespace pcw;

struct BookDirectoryBuilderFixture {
  TmpDir tmp;
  BookDirectoryBuilder builder;
  BookDirectoryBuilderFixture() : tmp(), builder(tmp.dir(), "test") {}
};

////////////////////////////////////////////////////////////////////////////////
BOOST_FIXTURE_TEST_SUITE(BookDirectoryBuilder, BookDirectoryBuilderFixture)

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(BuildAbbyyTest) {
  builder.add_zip_file_path(
      "misc/data/test/1841-DieGrenzboten-abbyy-small.zip");
  const auto book = builder.build();
  BOOST_REQUIRE(book);
  BOOST_REQUIRE(book->size() == 2);
  BOOST_CHECK(book->find(179392) != nullptr);
  BOOST_CHECK(book->find(179393) != nullptr);
  BOOST_CHECK_EQUAL(book->find(179392)->id(), 179392);
  BOOST_CHECK_EQUAL(book->find(179393)->id(), 179393);
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_SUITE_END()
