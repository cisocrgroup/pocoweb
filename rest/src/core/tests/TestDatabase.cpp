#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE TestDatabase

#include <boost/test/unit_test.hpp>
#include <iostream>
#include <sqlpp11/sqlpp11.h>
#include "MockDb.h"
#include "core/Tables.h"
#include "core/User.hpp"
#include "core/Password.hpp"
#include "core/BookBuilder.hpp"
#include "core/PageBuilder.hpp"
#include "core/LineBuilder.hpp"
#include "core/Book.hpp"
#include "core/Page.hpp"
#include "core/Line.hpp"
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
	PageSptr page;
	LineSptr line;

	BooksFixture() {
		LineBuilder lbuilder;
		lbuilder.set_box({2,3,4,5});
		lbuilder.set_image_path("image");
		lbuilder.append("text", 4, 1.0);
		line = lbuilder.build();

		PageBuilder pbuilder;
		pbuilder.set_image_path("image");
		pbuilder.set_ocr_path("ocr");
		pbuilder.set_box({1,2,3,4});
		pbuilder.append(*line);
		page = pbuilder.build();

		BookBuilder bbuilder;
		bbuilder.set_author("author");
		bbuilder.set_title("title");
		bbuilder.set_directory("directory");
		bbuilder.set_year(2017);
		bbuilder.set_uri("uri");
		bbuilder.set_language("language");
		bbuilder.set_description("description");
		bbuilder.set_owner(user);
		bbuilder.append(*page);
		book = bbuilder.build();
	}
};

////////////////////////////////////////////////////////////////////////////////
BOOST_FIXTURE_TEST_SUITE(Books, BooksFixture)

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(InsertBook)
{
	db.expect("UPDATE projects SET origin=0 WHERE (projects.projectid=0)");
	db.expect("INSERT INTO books (author,title,directory,year,uri,bookid,"
		"description,lang) VALUES('author','title','directory',2017,"
		"'uri',0,'description','language')");
	db.expect("INSERT INTO pages (bookid,pageid,imagepath,ocrpath,pleft,"
			"ptop,pright,pbottom) VALUES(0,1,'image','ocr',1,2,3,4)");
	db.expect("INSERT INTO textlines (bookid,pageid,lineid,imagepath,lleft,"
			"ltop,lright,lbottom) VALUES(0,1,1,'image',2,3,4,5)");
	// t
	db.expect("INSERT INTO contents (bookid,pageid,lineid,seq,ocr,cor,cut,conf) "
			"VALUES(0,1,1,0,116,0,1,1)");
	// e
	db.expect("INSERT INTO contents (bookid,pageid,lineid,seq,ocr,cor,cut,conf) "
			"VALUES(0,1,1,1,101,0,2,1)");
	// x
	db.expect("INSERT INTO contents (bookid,pageid,lineid,seq,ocr,cor,cut,conf) "
			"VALUES(0,1,1,2,120,0,3,1)");
	// t
	db.expect("INSERT INTO contents (bookid,pageid,lineid,seq,ocr,cor,cut,conf) "
			"VALUES(0,1,1,3,116,0,4,1)");
	auto view = insert_book(db, *book);
	BOOST_CHECK_EQUAL(view, book);
	db.validate();
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_SUITE_END()

