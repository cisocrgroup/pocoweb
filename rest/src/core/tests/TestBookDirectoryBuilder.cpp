#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE BookDirectoryBuilderTest

#include "core/Book.hpp"
#include "core/BookDirectoryBuilder.hpp"
#include "core/Page.hpp"
#include "core/PageBuilder.hpp"
#include "core/User.hpp"
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
  for (const auto &page : *book) {
    std::cout << "PAGE ID: " << page->id() << "\n";
  }
  BOOST_CHECK(book->find(1) != nullptr);
  BOOST_CHECK(book->find(2) != nullptr);
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_SUITE_END()
