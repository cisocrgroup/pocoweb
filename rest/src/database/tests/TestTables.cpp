#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE TestTables

#include <sqlpp11/sqlpp11.h>
#include <boost/test/unit_test.hpp>
#include <iostream>
#include "core/Config.hpp"
#include "database/Tables.h"
#include "utils/MockDb.h"

using namespace sqlpp;
using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(InsertNormalUser) {
	MockDb db;
	db.expect(
	    "INSERT INTO users (name,email,institute,passwd,admin) "
	    "VALUES('name','email','inst','passwd',0)");
	tables::Users users;
	auto stmt = insert_into(users).set(
	    users.name = "name", users.email = "email",
	    users.institute = "inst", users.passwd = "passwd",
	    users.admin = false);
	db(stmt);
	db.validate();
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(InsertAdmin) {
	MockDb db;
	db.expect(
	    "INSERT INTO users (name,email,institute,passwd,admin) "
	    "VALUES('name','email','inst','passwd',1)");
	tables::Users users;
	auto stmt =
	    insert_into(users).set(users.name = "name", users.email = "email",
				   users.institute = "inst",
				   users.passwd = "passwd", users.admin = true);
	db(stmt);
	db.validate();
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(SelectAllUser) {
	MockDb db;
	db.expect(
	    "SELECT "
	    "users.userid,users.name,users.email,users.institute,users.passwd,"
	    "users.admin "
	    "FROM users WHERE (users.email='testemail')");
	tables::Users users;
	auto stmt =
	    select(all_of(users)).from(users).where(users.email == "testemail");
	db(stmt);
	db.validate();
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(SelectUserId) {
	MockDb db;
	db.expect(
	    "SELECT users.userid FROM users WHERE (users.name='testname')");
	tables::Users users;
	auto stmt =
	    select(users.userid).from(users).where(users.name == "testname");
	db(stmt);
	db.validate();
}
