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
