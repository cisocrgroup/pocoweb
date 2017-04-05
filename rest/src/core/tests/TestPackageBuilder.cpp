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
		BookBuilder book_builder;
		book_builder.set_owner(*user);
		book_builder.append_text("first\npage");
		book_builder.append_text("second\npage");
		book_builder.append_text("third\npage");
		book_builder.append_text("fourth\npage");
		book_builder.append_text("fifth\npage");
		book_builder.append_text("sixth\npage");
		book_builder.append_text("seventh\npage");
		book_builder.append_text("eighth\npage");
		book_builder.append_text("ninth\npage");
		book_builder.append_text("tenth\npage");
		book = book_builder.build();
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

#define CHECK_FIRST_LINE(package, page, string) \
	do {\
		BOOST_REQUIRE(packages[package]->find(page));\
		BOOST_CHECK_EQUAL(packages[package]->find(page)->size(), 2);\
		BOOST_CHECK_EQUAL(packages[package]->find(page)->find(1)->cor(), string);\
	} while (false)

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
	CHECK_FIRST_LINE(0, 1, "first");
	CHECK_FIRST_LINE(1, 2, "second");
	CHECK_FIRST_LINE(2, 3, "third");
	CHECK_FIRST_LINE(3, 4, "fourth");
	CHECK_FIRST_LINE(4, 5, "fifth");
	CHECK_FIRST_LINE(0, 6, "sixth");
	CHECK_FIRST_LINE(1, 7, "seventh");
	CHECK_FIRST_LINE(2, 8, "eighth");
	CHECK_FIRST_LINE(3, 9, "ninth");
	CHECK_FIRST_LINE(4, 10, "tenth");
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
		BOOST_CHECK(package->size() >= 3);
	}
	CHECK_FIRST_LINE(0, 1, "first");
	CHECK_FIRST_LINE(1, 2, "second");
	CHECK_FIRST_LINE(2, 3, "third");
	CHECK_FIRST_LINE(0, 4, "fourth");
	CHECK_FIRST_LINE(1, 5, "fifth");
	CHECK_FIRST_LINE(2, 6, "sixth");
	CHECK_FIRST_LINE(0, 7, "seventh");
	CHECK_FIRST_LINE(1, 8, "eighth");
	CHECK_FIRST_LINE(2, 9, "ninth");
	CHECK_FIRST_LINE(0, 10, "tenth");
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(ContinousBuildWith5)
{
	builder.set_number(5);
	builder.set_continous(true);
	auto packages = builder.build();
	BOOST_CHECK_EQUAL(packages.size(), 5);
	for (const auto& package: packages) {
		BOOST_REQUIRE(package);
		BOOST_CHECK_EQUAL(package->size(), 2);
	}
	CHECK_FIRST_LINE(0, 1, "first");
	CHECK_FIRST_LINE(0, 2, "second");
	CHECK_FIRST_LINE(1, 3, "third");
	CHECK_FIRST_LINE(1, 4, "fourth");
	CHECK_FIRST_LINE(2, 5, "fifth");
	CHECK_FIRST_LINE(2, 6, "sixth");
	CHECK_FIRST_LINE(3, 7, "seventh");
	CHECK_FIRST_LINE(3, 8, "eighth");
	CHECK_FIRST_LINE(4, 9, "ninth");
	CHECK_FIRST_LINE(4, 10, "tenth");
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(ContinousBuildWith3)
{
	builder.set_number(3);
	builder.set_continous(true);
	auto packages = builder.build();
	BOOST_CHECK_EQUAL(packages.size(), 3);
	for (const auto& package: packages) {
		BOOST_REQUIRE(package);
		BOOST_CHECK(package->size() <= 4);
		BOOST_CHECK(package->size() >= 3);
	}
	CHECK_FIRST_LINE(0, 1, "first");
	CHECK_FIRST_LINE(0, 2, "second");
	CHECK_FIRST_LINE(0, 3, "third");
	CHECK_FIRST_LINE(0, 4, "fourth");
	CHECK_FIRST_LINE(1, 5, "fifth");
	CHECK_FIRST_LINE(1, 6, "sixth");
	CHECK_FIRST_LINE(1, 7, "seventh");
	CHECK_FIRST_LINE(2, 8, "eighth");
	CHECK_FIRST_LINE(2, 9, "ninth");
	CHECK_FIRST_LINE(2, 10, "tenth");
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(RandomBuildWith5)
{
	builder.set_number(5);
	builder.set_random(true);
	auto packages = builder.build();
	BOOST_CHECK_EQUAL(packages.size(), 5);
	for (const auto& package: packages) {
		BOOST_REQUIRE(package);
		BOOST_CHECK_EQUAL(package->size(), 2);
	}
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(RandomBuildWith3)
{
	builder.set_number(3);
	builder.set_random(true);
	auto packages = builder.build();
	BOOST_CHECK_EQUAL(packages.size(), 3);
	for (const auto& package: packages) {
		BOOST_REQUIRE(package);
		BOOST_CHECK(package->size() <= 4);
		BOOST_CHECK(package->size() >= 3);
	}
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_SUITE_END()

#undef CHECK_FIRST_LINE
