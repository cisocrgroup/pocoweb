#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE TestDatabase

#include <boost/test/unit_test.hpp>
#include <iostream>
#include <sqlpp11/sqlpp11.h>
#include "MockDb.h"
#include "core/Tables.h"
#include "core/User.hpp"
#include "core/Password.hpp"
#include "core/Book.hpp"
#include "core/Page.hpp"
#include "core/NewDatabase.hpp"

using namespace sqlpp;
using namespace pcw;

struct UsersFixture {
	UserSptr user;
	MockDb db;
	UsersFixture()
		: user(std::make_shared<User>("name", "email", "institute", 42))
		, db()
	{}
};

////////////////////////////////////////////////////////////////////////////////
BOOST_FIXTURE_TEST_SUITE(Users, UsersFixture)

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(CreateUser)
{
	auto nuser = create_user(db, user->name, "password", user->email, user->institute);
	db.expect(std::regex(R"(INSERT INTO users \(name,email,institute,passwd\) )"
				R"(VALUES\('name','email','institute','.+'\))"));
	BOOST_REQUIRE(nuser);
	BOOST_CHECK_EQUAL(user->name, nuser->name);
	BOOST_CHECK_EQUAL(user->email, nuser->email);
	BOOST_CHECK_EQUAL(user->institute, nuser->institute);
	db.validate();
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(LoginUser)
{
	db.expect(std::regex(R"(SELECT .* FROM users WHERE \(users.name='name'\))"));
	login_user(db, user->name, "password");
	db.validate();
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(UpdateUser)
{
	db.expect("UPDATE users SET email='email',institute='institute' "
			"WHERE (users.userid=42)");
	update_user(db, *user);
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
	select_user(db, user->id());
	db.validate();
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(DeleteUserByName)
{
	db.expect("DELETE FROM users WHERE (users.name='name')");
	delete_user(db, user->name);
	db.validate();
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(DeleteUserById)
{
	db.expect("DELETE FROM users WHERE (users.userid=42)");
	delete_user(db, user->id());
	db.validate();
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_SUITE_END()

struct BooksFixture: public UsersFixture {
	BookSptr book;
	BooksFixture(): UsersFixture(), book() {
		book = std::make_shared<Book>();
		book->set_owner(*user);
	}
};

////////////////////////////////////////////////////////////////////////////////
BOOST_FIXTURE_TEST_SUITE(Books, BooksFixture)

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(InsertProject)
{
	db.expect("UPDATE projects SET origin=0 WHERE (projects.projectid=0)");
	auto view = insert_project(db, *book);
	db.validate();
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(InsertBook)
{
	db.expect("INSERT INTO books (author,title,directory,year,uri,bookid,"
		"description,lang) VALUES('','','',0,'',0,'','')");
	auto same = insert_book(db, *book);
	BOOST_CHECK_EQUAL(same, book);
	db.validate();
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(InsertPage)
{
	db.expect("INSERT INTO pages (bookid,pageid,imagepath,ocrpath,pleft,"
			"ptop,pright,pbottom) VALUES(0,13,'','',0,0,0,0)");
	auto page = std::make_shared<Page>(13);
	book->push_back(*page);
	auto same = insert_book(db, *book);
	BOOST_CHECK_EQUAL(same, book);
	db.validate();
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_SUITE_END()

