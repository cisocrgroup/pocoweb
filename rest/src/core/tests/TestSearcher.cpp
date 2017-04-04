#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE SearcherTest

#include <boost/test/unit_test.hpp>
#include <iostream>
#include "utils/Error.hpp"
#include "core/PageBuilder.hpp"
#include "core/Searcher.hpp"
#include "core/BookBuilder.hpp"
#include "core/PageBuilder.hpp"
#include "core/LineBuilder.hpp"
#include "core/Package.hpp"
#include "core/Book.hpp"
#include "core/Page.hpp"
#include "core/User.hpp"

using namespace pcw;

struct SearcherFixture {
	Searcher searcher;
	BookSptr book;
	UserSptr user;
	SearcherFixture(): searcher(), book(), user() {
		user = std::make_shared<User>("name", "pass", "email", "inst", 42);
		BookBuilder bbuilder;
		PageBuilder pbuilder;
		LineBuilder lbuilder;

		lbuilder.append("This is the first line of the first page.", 100, 1);
		pbuilder.append(*lbuilder.build());
		lbuilder.reset();
		lbuilder.append("This is the second line of the first page.", 100, 1);
		pbuilder.append(*lbuilder.build());
		lbuilder.reset();
		lbuilder.append("This is the third line of the first page.", 100, 1);
		pbuilder.append(*lbuilder.build());
		bbuilder.append(*pbuilder.build());

		pbuilder.reset();
		lbuilder.append("Dies ist die erste Zeile der zweiten Seite.", 100, 1);
		pbuilder.append(*lbuilder.build());
		lbuilder.reset();
		lbuilder.append("Dies ist die zweite Zeile der zweiten Seite.", 100, 1);
		pbuilder.append(*lbuilder.build());
		lbuilder.reset();
		lbuilder.append("Dies ist die dritte Zeile der zweiten Seite.", 100, 1);
		pbuilder.append(*lbuilder.build());
		bbuilder.append(*pbuilder.build());

		book = bbuilder.build();
		searcher.set_project(*book);
	}
};

////////////////////////////////////////////////////////////////////////////////
BOOST_FIXTURE_TEST_SUITE(Searcher, SearcherFixture)

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(TestNoResults)
{
	auto res = searcher.find([](const auto&){return false;});
	BOOST_CHECK_EQUAL(res.size(), 0);
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(TestAllResults)
{
	auto res = searcher.find([](const auto&){return true;});
	BOOST_CHECK_EQUAL(res.size(), 6);
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_SUITE_END()
