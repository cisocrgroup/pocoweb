#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE TestDatabase

#include <boost/test/unit_test.hpp>
#include <iostream>
#include <sqlpp11/sqlpp11.h>
#include "MockDb.h"
#include "core/Tables.h"
#include "core/User.hpp"
#include "core/Password.hpp"
#include "core/NewDatabase.hpp"

using namespace sqlpp;
using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(CreateUser)
{
	MockDb db;
	db.expect(std::regex(R"(INSERT INTO users \(.+\) VALUES\(.+\))"));
	auto user = create_user(db, "name", "password", "email", "institute");
	BOOST_REQUIRE(user);
	BOOST_CHECK_EQUAL(user->name, "name");
	BOOST_CHECK_EQUAL(user->email, "email");
	BOOST_CHECK_EQUAL(user->institute, "institute");
	db.validate();
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(LoginUser)
{
	MockDb db;
	db.expect(std::regex(R"(SELECT .* FROM users WHERE \(users.name='name'\))"));
	auto user = login_user(db, "name", "password");
	db.validate();
}

