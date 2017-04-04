#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE PackageBuilderTest

#include <boost/test/unit_test.hpp>
#include <iostream>
#include "utils/Error.hpp"
#include "core/PageBuilder.hpp"
#include "core/PackageBuilder.hpp"
#include "core/BookBuilder.hpp"
#include "core/PageBuilder.hpp"
#include "core/Package.hpp"
#include "core/Book.hpp"
#include "core/Page.hpp"
#include "core/User.hpp"

using namespace pcw;

struct PackageBuilderFixture {
	PackageBuilder builder;
	BookSptr book;
	UserSptr user;
	size_t N;
	PackageBuilderFixture(): builder(), book(), user(), N(10) {
		user = std::make_shared<User>("name", "pass", "email", "inst", 42);
		BookBuilder bbuilder;
		PageBuilder pbuilder;
		for (size_t i = 0; i < N; ++i)
			bbuilder.append(*pbuilder.build());
		bbuilder.set_owner(*user);
		book = bbuilder.build();
		builder.set_project(*book);
	}
};

////////////////////////////////////////////////////////////////////////////////
BOOST_FIXTURE_TEST_SUITE(PackageBuilder, PackageBuilderFixture)

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(InvalidNumberTooSmall)
{
	builder.set_number(0);
	BOOST_CHECK_THROW(builder.build(), pcw::Error);
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(InvalidNumberTooBig)
{
	builder.set_number(N+1);
	BOOST_CHECK_THROW(builder.build(), pcw::Error);
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(SimpleBuildWith5)
{
	builder.set_number(5);
	auto packages = builder.build();
	BOOST_CHECK_EQUAL(packages.size(), 5);
	for (const auto& package: packages) {
		BOOST_REQUIRE(package);
		BOOST_CHECK_EQUAL(package->size(), 2);
	}
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(SimpleBuildWith3)
{
	builder.set_number(3);
	auto packages = builder.build();
	BOOST_CHECK_EQUAL(packages.size(), 3);
	for (const auto& package: packages) {
		BOOST_REQUIRE(package);
		BOOST_CHECK(package->size() <= 4);
	}
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_SUITE_END()
