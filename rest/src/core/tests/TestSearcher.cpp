#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE SearcherTest

#include <boost/test/unit_test.hpp>
#include <iostream>
#include "core/Book.hpp"
#include "core/BookBuilder.hpp"
#include "core/LineBuilder.hpp"
#include "core/Package.hpp"
#include "core/Page.hpp"
#include "core/PageBuilder.hpp"
#include "core/PageBuilder.hpp"
#include "core/Searcher.hpp"
#include "core/User.hpp"
#include "utils/Error.hpp"

using namespace pcw;

struct SearcherFixture {
	Searcher searcher;
	BookSptr book;
	UserSptr user;
	SearcherFixture() : searcher(), book(), user() {
		BookBuilder builder;
		user =
		    std::make_shared<User>("name", "pass", "email", "inst", 42);
		builder.set_owner(*user);
		builder.append_text(
		    "This is the first line of the first page.\n"
		    "This is the second line of the first page.\n"
		    "This is the third line of the first page.\n");
		builder.append_text(
		    "Mögen hätt ich schon gewollt,\n"
		    "aber dürfen\n"
		    "hab ich mich nicht getraut.\n");
		// builder.append_text(
		//     "ſunt Alexcandri aut Cæſaris credere recuſarem, non puto
		//     "
		//     "illos merito");
		book = builder.build();
		searcher.set_project(*book);
	}
};

////////////////////////////////////////////////////////////////////////////////
BOOST_FIXTURE_TEST_SUITE(Searcher, SearcherFixture)

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(TestDefaults) {
	BOOST_CHECK(searcher.match_words());
	BOOST_CHECK(searcher.ignore_case());
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(TestNoResults) {
	auto res = searcher.find_impl([](const auto&) { return false; });
	BOOST_CHECK_EQUAL(res.size(), 0);
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(TestAllResults) {
	auto res = searcher.find_impl([](const auto&) { return true; });
	BOOST_CHECK_EQUAL(res.size(), 6);
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(TestSearchByPageId) {
	auto res = searcher.find_impl(
	    [](const auto& line) { return line.page().id() == 1; });
	BOOST_CHECK_EQUAL(res.size(), 3);
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(TestSearchByLineId) {
	auto res =
	    searcher.find_impl([](const auto& line) { return line.id() == 1; });
	BOOST_CHECK_EQUAL(res.size(), 2);
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(TestSearchByRegex_first_page) {
	auto res = searcher.find("first page");
	BOOST_CHECK_EQUAL(res.size(), 3);
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(TestSearchByRegex_nothing) {
	auto res = searcher.find("nothing");
	BOOST_CHECK_EQUAL(res.size(), 0);
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(TestSearchByRegexIgnoreCase) {
	auto res = searcher.find("mögen");
	BOOST_CHECK_EQUAL(res.size(), 1);
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(TestSearchByRegexDoNotIgnoreCase) {
	searcher.set_ignore_case(false);
	auto res = searcher.find("mögen");
	BOOST_CHECK_EQUAL(res.size(), 0);
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(TestSearchByRegexMatchOnWordBoundaries) {
	auto res = searcher.find("s.*n");  // matches schon but *not* second ...
	BOOST_CHECK_EQUAL(res.size(), 1);
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(TestSearchByRegexMatchFullLines) {
	searcher.set_match_words(false);
	auto res = searcher.find("s.*n");
	BOOST_CHECK_EQUAL(res.size(), 4);
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_SUITE_END()
