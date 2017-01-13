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

struct UsersFixture {
	MockDb db;
	UsersFixture(): db() {}
};

////////////////////////////////////////////////////////////////////////////////
BOOST_FIXTURE_TEST_SUITE(Users, UsersFixture)

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(CreateUser)
{
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
	db.expect(std::regex(R"(SELECT .* FROM users WHERE \(users.name='name'\))"));
	login_user(db, "name", "password");
	db.validate();
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(UpdateUser)
{
	User user("name", "email", "institute", 42);
	db.expect("UPDATE users SET email='email',institute='institute' "
			"WHERE (users.userid=42)");
	update_user(db, user);
	db.validate();
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(SelectUserByName)
{
	db.expect(std::regex(R"(SELECT .* FROM users WHERE \(users.name='name'\))"));
	select_user(db, "name");
	db.validate();
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(SelectUserById)
{
	db.expect(std::regex(R"(SELECT .* FROM users WHERE \(users.userid=42\))"));
	select_user(db, 42);
	db.validate();
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(DeleteUserByName)
{
	db.expect("DELETE FROM users WHERE (users.name='name')");
	delete_user(db, "name");
	db.validate();
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(DeleteUserById)
{
	db.expect("DELETE FROM users WHERE (users.userid=42)");
	delete_user(db, 42);
	db.validate();
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_SUITE_END()

