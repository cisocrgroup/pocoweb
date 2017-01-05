#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE TestTables

#include <boost/test/unit_test.hpp>
#include <iostream>
#include <sqlpp11/sqlpp11.h>
#include <sqlpp11/mysql/mysql.h>
#include <sqlpp11/mysql/mysql.h>
#include "core/Tables.h"
#include "core/Config.hpp"

using namespace sqlpp;
using namespace pcw;

struct TestDbConfig {
	std::shared_ptr<mysql::connection_config> config;
	TestDbConfig(): config(std::make_shared<mysql::connection_config>()) {
		auto c = Config::load("config.ini");
		config->user = c.db.user;
		config->host = c.db.host;
		config->database = c.db.db;
		config->password = c.db.pass;
		config->debug = true;
	}
};

struct TableUsersFixture {
	TestDbConfig config;
	mysql::connection db;
	tables::Users users;
	// Tables::Users::Table users;
	TableUsersFixture(): config(), db(config.config) {
		db(remove_from(users).where(users.name == "first name"));
		db(insert_into(users).set(
			users.name = "first name",
			users.email = "first email",
			users.institute = "first insitute",
			users.passwd = "first password"
		));
	}
	~TableUsersFixture() noexcept {
		try {
			db(remove_from(users).where(users.name == "first name"));
		} catch (...) {
			BOOST_FAIL("SQL error while removing users");
		}
	}
};

////////////////////////////////////////////////////////////////////////////////
BOOST_FIXTURE_TEST_SUITE(TableUsers, TableUsersFixture)

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(FirstUser)
{
	/*auto res = db(select(users.userid, users.name).
			from(users).
			where(users.userid == 1));*/
	auto user = db(select(users.userid).from(users).where(users.name == "first name"));
	BOOST_REQUIRE(not user.empty());
	// BOOST_CHECK_EQUAL(not user.front().name, "first name");
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_SUITE_END()
