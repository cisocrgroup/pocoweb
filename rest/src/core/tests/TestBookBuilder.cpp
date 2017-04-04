#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE BookBuilderTest

#include <boost/test/unit_test.hpp>
#include <iostream>
#include "core/PageBuilder.hpp"
#include "core/BookBuilder.hpp"
#include "core/Book.hpp"
#include "core/Page.hpp"
#include "core/User.hpp"

using namespace pcw;

struct BookBuilderFixture {
	BookBuilder builder;
};

////////////////////////////////////////////////////////////////////////////////
BOOST_FIXTURE_TEST_SUITE(BookBuilder, BookBuilderFixture)

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(ResetTest)
{
	auto first = builder.build();
	builder.reset();
	auto second = builder.build();
	BOOST_CHECK(first != second);
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(AuthorTest)
{
	builder.set_author("author");
	BOOST_CHECK_EQUAL(builder.build()->author, "author");
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(TitleTest)
{
	builder.set_title("title");
	BOOST_CHECK_EQUAL(builder.build()->title, "title");
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(DescriptionTest)
{
	builder.set_description("description");
	BOOST_CHECK_EQUAL(builder.build()->description, "description");
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(UriTest)
{
	builder.set_uri("uri");
	BOOST_CHECK_EQUAL(builder.build()->uri, "uri");
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(LanguageTest)
{
	builder.set_language("language");
	BOOST_CHECK_EQUAL(builder.build()->lang, "language");
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(DirectoryTest)
{
	builder.set_directory("/directory/");
	BOOST_CHECK_EQUAL(builder.build()->dir, "/directory/");
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(YearTest)
{
	builder.set_year(2017);
	BOOST_CHECK_EQUAL(builder.build()->year, 2017);
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(OwnerTest)
{
	builder.set_owner(*std::make_shared<User>("", "", "", "", 42));
	BOOST_CHECK_EQUAL(builder.build()->owner().id(), 42);
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(IdTest)
{
	builder.set_id(42);
	BOOST_CHECK_EQUAL(builder.build()->id(), 42);
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(AppendTest)
{
	std::vector<PageSptr> pages(5);
	PageBuilder pbuilder;
	for (auto& p: pages) {
		pbuilder.reset();
		p = pbuilder.build();
		builder.append(*p);
	}

	auto b = builder.build();
	BOOST_CHECK_EQUAL(b->size(), 5);

	int i = 1;
	for (const auto& p: pages) {
		BOOST_CHECK_EQUAL(p->id(), i);
		BOOST_CHECK_EQUAL(&p->book(), b.get());
		BOOST_CHECK_EQUAL(b->find(i), p);
		++i;
	}
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_SUITE_END()
