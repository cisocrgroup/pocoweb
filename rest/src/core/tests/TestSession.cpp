#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE SessionTest

#include <boost/test/unit_test.hpp>
#include <iostream>
#include <thread>
#include "core/BookBuilder.hpp"
#include "core/ProjectBuilder.hpp"
#include "core/Session.hpp"
#include "core/User.hpp"
#include "database/ConnectionPool.hpp"
#include "utils/MockDb.h"

using namespace pcw;

struct SessionFixture {
	ConnectionPoolConnection<MockDb> mock;
	Connection<MockDb> connection;
	BookSptr book;
	ProjectSptr project;
	UserSptr user;
	SessionSptr session;

	SessionFixture()
	    : mock(), connection(&mock), book(), project(), user(), session() {
		mock.in_use = true;
		user =
		    std::make_shared<User>("name", "pass", "email", "inst", 42);
		BookBuilder bbuilder;
		bbuilder.set_owner(*user);
		book = bbuilder.build();
		ProjectBuilder pbuilder;
		pbuilder.set_origin(*book);
		project = pbuilder.build();
		session = std::make_shared<Session>(
		    *user, std::make_shared<AppCache>(2, 2, 2));
	}
};

////////////////////////////////////////////////////////////////////////////////
BOOST_FIXTURE_TEST_SUITE(SessionTest, SessionFixture)

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(HasUser) {
	BOOST_CHECK_EQUAL(&session->user(), user.get());
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(WorksWithNullCache) {
	// just check for segfaults
	session = std::make_shared<Session>(*user, nullptr);
	session->insert_project(connection, *book);
	session->insert_project(connection, *project);
	auto b = session->find_project(connection, book->id());
	auto p = session->find_project(connection, project->id());
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(HasRandomSessionId) {
	session = std::make_shared<Session>(*user, nullptr);
	BOOST_CHECK(not session->id().empty());
	BOOST_CHECK_EQUAL(session->id().size(), 16);
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(InsertBooks) {
	session->insert_project(connection, *book);
	session->insert_project(connection, *book);
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(InsertProjects) {
	session->insert_project(connection, *project);
	session->insert_project(connection, *project);
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(InsertBooksAndProjects) {
	session->insert_project(connection, *book);
	session->insert_project(connection, *project);
	session->insert_project(connection, *book);
	session->insert_project(connection, *project);
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(FindBooks) {
	session->insert_project(connection, *book);
	auto tmp = session->find_project(connection, book->id());
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(FindProjects) {
	session->insert_project(connection, *project);
	auto tmp = session->find_project(connection, project->id());
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_SUITE_END()
