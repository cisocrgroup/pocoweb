#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE TestTables

#include <boost/test/unit_test.hpp>
#include <iostream>
#include <sqlpp11/sqlpp11.h>
#include "MockDb.h"
#include "core/Tables.h"
#include "core/Config.hpp"

using namespace sqlpp;
using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(InsertUser)
{
	MockDb db;
	db.expect("INSERT INTO users (name,email,institute,passwd) "
		"VALUES('name','email','inst','passwd')");
	tables::Users users;
	auto stmt = insert_into(users).set(
		users.name = "name",
		users.email = "email",
		users.institute = "inst",
		users.passwd = "passwd"
	);
	db(stmt);
	db.validate();
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(SelectAllUser)
{
	MockDb db;
	db.expect("SELECT users.userid,users.name,users.email,users.institute,users.passwd "
			"FROM users WHERE (users.email='testemail')");
	tables::Users users;
	auto stmt = select(all_of(users)).from(users).
		where(users.email == "testemail");
	db(stmt);
	db.validate();
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(SelectUserId)
{
	MockDb db;
	db.expect("SELECT users.userid FROM users WHERE (users.name='testname')");
	tables::Users users;
	auto stmt = select(users.userid).from(users).where(users.name == "testname");
	db(stmt);
	db.validate();
}
