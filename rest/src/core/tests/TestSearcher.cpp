#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE SearcherTest

#include "core/Book.hpp"
#include "core/BookBuilder.hpp"
#include "core/LineBuilder.hpp"
#include "core/Package.hpp"
#include "core/Page.hpp"
#include "core/PageBuilder.hpp"
#include "core/Searcher.hpp"
#include "utils/Error.hpp"
#include <boost/test/unit_test.hpp>
#include <iostream>

template <class M> size_t msize(const M &m) {
  size_t n = 0;
  for (const auto &kv : m.matches) {
    n += kv.second.size();
  }
  return n;
}

using namespace pcw;

struct SearcherFixture {
  Searcher searcher;
  BookSptr book;
  int user;
  SearcherFixture() : searcher(), book(), user() {
    BookBuilder builder;
    user = 42;
    builder.set_owner(user);
    builder.append_text("This is the first line of the first page.\n"
                        "This is the second line of the first page.\n"
                        "This is the third line of the first page.\n");
    builder.append_text("Mögen hätt ich schon gewollt,\n"
                        "aber dürfen\n"
                        "hab ich mich nicht getraut.\n");
    builder.append_text(
        "ſunt Alexcandri aut Cæſaris credere recuſarem, non puto "
        "illos merito\nſunt ad Bellum, parata habeant. Sed Civibus "
        "data ſunt à Natura\naſunt ſunta ſſuntſ.");
    book = builder.build();
    searcher.set_project(*book);
    searcher.set_max(1000);
  }
};

////////////////////////////////////////////////////////////////////////////////
BOOST_FIXTURE_TEST_SUITE(Searcher, SearcherFixture)

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(TestDefaults) { BOOST_CHECK(searcher.ignore_case()); }

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(TestNoResults) {
  const auto res = searcher.find_impl([](const auto &) { return false; });
  BOOST_CHECK_EQUAL(msize(res), 0);
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(TestAllResults) {
  const auto res = searcher.find_impl([](const auto &) { return true; });
  BOOST_CHECK_EQUAL(msize(res), 63);
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(TestAllResultsWithMax) {
  searcher.set_max(10);
  const auto res = searcher.find_impl([](const auto &) { return true; });
  BOOST_CHECK_EQUAL(msize(res), 10);
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(TestAllResultsWithMaxAndSkip) {
  searcher.set_max(10);
  searcher.set_skip(60);
  const auto res = searcher.find_impl([](const auto &) { return true; });
  BOOST_CHECK_EQUAL(msize(res), 3);
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(TestSearchByPageId) {
  const auto res = searcher.find_impl(
      [](const auto &token) { return token.line->page().id() == 1; });
  BOOST_CHECK_EQUAL(msize(res), 27);
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(TestSearchByLineId) {
  const auto res = searcher.find_impl(
      [](const auto &token) { return token.line->id() == 1; });
  BOOST_CHECK_EQUAL(msize(res), 24);
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(TestSearchByRegex_nothing) {
  const auto res = searcher.find("nothing");
  BOOST_CHECK_EQUAL(msize(res), 0);
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(TestSearchByRegexIgnoreCase) {
  const auto res = searcher.find("mögen");
  BOOST_CHECK_EQUAL(msize(res), 1);
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(TestSearchByRegexDoNotIgnoreCase) {
  searcher.set_ignore_case(false);
  const auto res = searcher.find("mögen");
  BOOST_CHECK_EQUAL(msize(res), 0);
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(TestSearchByRegexMatchAtLineBeginIgnoreCase) {
  const auto res = searcher.find("this");
  BOOST_CHECK_EQUAL(msize(res), 3);
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(TestSearchByRegexMatchAtLineBegin) {
  const auto res = searcher.find("This");
  BOOST_CHECK_EQUAL(msize(res), 3);
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(TestSearchWithLigatureAE) {
  const auto res = searcher.find("Cæſaris");
  BOOST_CHECK_EQUAL(msize(res), 1);
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(TestSearchWithLongS) {
  const auto res = searcher.find("ſunt");
  BOOST_CHECK_EQUAL(msize(res), 3);
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(TestSearchWithSkipTotalCount) {
  searcher.set_skip(1);
  searcher.set_max(1);
  const auto res = searcher.find("this");
  BOOST_CHECK_EQUAL(msize(res), 1);
  BOOST_CHECK_EQUAL(res.totalCount, 3);
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_SUITE_END()
