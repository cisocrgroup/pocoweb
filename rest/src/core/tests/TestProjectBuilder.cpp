#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE ProjectBuilderTest

#include <boost/test/unit_test.hpp>
#include <iostream>
#include "core/PageBuilder.hpp"
#include "core/ProjectBuilder.hpp"
#include "core/BookBuilder.hpp"
#include "core/PageBuilder.hpp"
#include "core/BookView.hpp"
#include "core/Book.hpp"
#include "core/Page.hpp"
#include "core/User.hpp"

using namespace pcw;

struct ProjectBuilderFixture {
	ProjectBuilder builder;
	BookSptr book;
	UserSptr user;
	ProjectBuilderFixture(): builder(), book(), user() {
		user = std::make_shared<User>("name", "pass", "email", "inst", 42);
		BookBuilder bbuilder;
		PageBuilder pbuilder;
		bbuilder.append(*pbuilder.build());
		pbuilder.reset();
		bbuilder.append(*pbuilder.build());
		bbuilder.set_owner(*user);
		book = bbuilder.build();
		builder.set_book(*book);
	}
};

////////////////////////////////////////////////////////////////////////////////
BOOST_FIXTURE_TEST_SUITE(ProjectBuilder, ProjectBuilderFixture)

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(Reset)
{
	auto first = builder.build();
	builder.reset();
	auto second = builder.build();
	BOOST_CHECK(first != second);
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(Empty)
{
	auto project = builder.build();
	BOOST_CHECK(project->empty());
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(Origin)
{
	auto project = builder.build();
	BOOST_CHECK_EQUAL(&project->origin(), book.get());
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(Owner)
{
	auto project = builder.build();
	BOOST_CHECK_EQUAL(&project->owner(), user.get());
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(AddOne)
{
	builder.add_page(1);
	auto project = builder.build();
	BOOST_CHECK_EQUAL(project->size(), 1);
	BOOST_CHECK_EQUAL(project->find(1), book->find(1));
	BOOST_CHECK(not project->find(2));
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(AddTwo)
{
	builder.add_page(1);
	builder.add_page(2);
	auto project = builder.build();
	BOOST_CHECK_EQUAL(project->size(), 2);
	BOOST_CHECK_EQUAL(project->find(1), book->find(1));
	BOOST_CHECK_EQUAL(project->find(2), book->find(2));
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(AddEqual)
{
	builder.add_page(1);
	builder.add_page(1);
	auto project = builder.build();
	BOOST_CHECK_EQUAL(project->size(), 1);
	BOOST_CHECK_EQUAL(project->find(1), book->find(1));
	BOOST_CHECK(not project->find(2));
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(Order)
{
	builder.add_page(2);
	builder.add_page(1);
	auto project = builder.build();
	BOOST_CHECK_EQUAL(project->size(), 2);
	BOOST_CHECK_EQUAL(project->find(1), book->find(1));
	BOOST_CHECK_EQUAL(project->find(2), book->find(2));
	BOOST_CHECK_EQUAL((*(*project->begin())).id(), 1);
	BOOST_CHECK_EQUAL((*(*std::prev(project->end()))).id(), 2);
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(AddInvalid)
{
	BOOST_CHECK_THROW(builder.add_page(3), std::runtime_error);
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_SUITE_END()
