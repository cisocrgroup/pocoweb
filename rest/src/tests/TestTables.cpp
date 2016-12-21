#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE TestTables

#include <boost/test/unit_test.hpp>
#include <iostream>
#include <sqlpp11/sqlpp11.h>
#include <sqlpp11/container/connection.h>
#include "core/Tables.hpp"

namespace sql = sqlpp::container;
using namespace pcw;

struct TableUsersFixture {
	using Users = std::vector<Tables::Users::Struct>;
	sql::connection<Users> db;
	Tables::Users::Table users;
	TableUsersFixture(): db({}), users() {
		db(sqlpp::insert_into(users).set(
				users.userid = 1,
				users.name = "first name"));
		db(sqlpp::insert_into(users).set(
				users.userid = 2,
				users.name = "second name"));
	}
};

////////////////////////////////////////////////////////////////////////////////
BOOST_FIXTURE_TEST_SUITE(TableUsers, TableUsersFixture)

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(FirstUser)
{
	/*
	const auto& user = db(
			sqlpp::select(users.userid).
			from(users).
			where(users.userid == 1));
	*/
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_SUITE_END()
