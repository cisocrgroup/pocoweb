#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE SessionStoreTest

#include <boost/test/unit_test.hpp>
#include <iostream>
#include "core/BookBuilder.hpp"
#include "core/ProjectBuilder.hpp"
#include "core/SessionStore.hpp"
#include "core/User.hpp"
#include "utils/MockDb.h"
#include "database/ConnectionPool.hpp"

using namespace pcw;

struct SessionStoreFixture {
	SessionStore session_store;

	SessionStoreFixture(): session_store(2) {}
};

////////////////////////////////////////////////////////////////////////////////
BOOST_FIXTURE_TEST_SUITE(SessionStoreTest, SessionStoreFixture)

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(Size)
{
	BOOST_CHECK_EQUAL(session_store.size(), 2);
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_SUITE_END()
