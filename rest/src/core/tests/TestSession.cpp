#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE SessionTest

#include <boost/test/unit_test.hpp>
#include <iostream>
#include "core/BookBuilder.hpp"
#include "core/ProjectBuilder.hpp"
#include "core/Session.hpp"
#include "core/User.hpp"
#include "utils/MockDb.h"
#include "database/ConnectionPool.hpp"

using namespace pcw;

struct SessionFixture {
	ConnectionPoolConnection<MockDb> mock;
	Connection<MockDb> connection;
	BookSptr book;
	BookViewSptr project;
	UserSptr user;
	AppCacheSptr cache;

	SessionFixture()
		: mock()
		, connection(&mock)
		, book()
		, project()
		, user()
		, cache(std::make_shared<AppCache>(2, 2))
	{
		mock.in_use = true;
		user = std::make_shared<User>("name", "email", "inst", 42);
		BookBuilder bbuilder;
		bbuilder.set_owner(*user);
		book = bbuilder.build();
		ProjectBuilder pbuilder;
		pbuilder.set_book(*book);
		project = pbuilder.build();
	}
};

////////////////////////////////////////////////////////////////////////////////
BOOST_FIXTURE_TEST_SUITE(SessionTest, SessionFixture)

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(WorksWithNullCache)
{
	// just check for segfaults
	Session s(*user);
	s.insert_project(connection, *book);
	s.insert_project(connection, *project);
	auto b = s.find_project(connection, book->id());
	auto p = s.find_project(connection, project->id());
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_SUITE_END()
