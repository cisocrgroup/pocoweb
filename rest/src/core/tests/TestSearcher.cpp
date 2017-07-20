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
		builder.append_text(
		    "ſunt Alexcandri aut Cæſaris credere recuſarem, non "
		    "puto "
		    "illos merito\nſunt ad Bellum, parata habeant. Sed Civibus "
		    "data ſunt à Natura\naſunt ſunta ſſuntſ.");
		book = builder.build();
		searcher.set_project(*book);
	}
};

////////////////////////////////////////////////////////////////////////////////
BOOST_FIXTURE_TEST_SUITE(Searcher, SearcherFixture)

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(TestDefaults) { BOOST_CHECK(searcher.ignore_case()); }

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(TestNoResults) {
	const auto res = searcher.find_impl([](const auto&) { return false; });
	BOOST_CHECK_EQUAL(res.size(), 0);
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(TestAllResults) {
	const auto res = searcher.find_impl([](const auto&) { return true; });
	BOOST_CHECK_EQUAL(res.size(), 9);
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(TestSearchByPageId) {
	const auto res = searcher.find_impl(
	    [](const auto& token) { return token.line->page().id() == 1; });
	BOOST_CHECK_EQUAL(res.size(), 3);
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(TestSearchByLineId) {
	const auto res = searcher.find_impl(
	    [](const auto& token) { return token.line->id() == 1; });
	BOOST_CHECK_EQUAL(res.size(), 3);
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(TestSearchByRegex_nothing) {
	const auto res = searcher.find("nothing");
	BOOST_CHECK_EQUAL(res.size(), 0);
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(TestSearchByRegexIgnoreCase) {
	const auto res = searcher.find("mögen");
	BOOST_CHECK_EQUAL(res.size(), 1);
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(TestSearchByRegexDoNotIgnoreCase) {
	searcher.set_ignore_case(false);
	const auto res = searcher.find("mögen");
	BOOST_CHECK_EQUAL(res.size(), 0);
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(TestSearchByRegexMatchAtLineBeginIgnoreCase) {
	const auto res = searcher.find("this");
	BOOST_CHECK_EQUAL(res.size(), 3);
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(TestSearchByRegexMatchAtLineBegin) {
	const auto res = searcher.find("This");
	BOOST_CHECK_EQUAL(res.size(), 3);
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(TestSearchWithLigatureAE) {
	const auto res = searcher.find("Cæſaris");
	BOOST_CHECK_EQUAL(res.size(), 1);
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(TestSearchWithLongS) {
	const auto res = searcher.find("ſunt");
	BOOST_CHECK_EQUAL(res.size(), 2);
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_SUITE_END()
