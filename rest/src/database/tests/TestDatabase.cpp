#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE TestDatabase

#include <boost/test/unit_test.hpp>
#include <iostream>
#include <sqlpp11/sqlpp11.h>
#include "utils/MockDb.h"
#include "core/User.hpp"
#include "core/Password.hpp"
#include "core/Book.hpp"
#include "core/Page.hpp"
#include "core/Line.hpp"
#include "database/Tables.h"
#include "database/Database.hpp"

using namespace sqlpp;
using namespace pcw;

struct UsersFixture {
	UserSptr user;
	MockDb db;
	UsersFixture()
		: user(std::make_shared<User>("name", "pass", "email", "institute", 42))
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
BOOST_AUTO_TEST_CASE(SelectProjectEntry)
{
	db.expect("SELECT projects.projectid,projects.origin,projects.owner,projects.pages "
			"FROM projects WHERE (projects.projectid=42)");
	// db.expect("SELECT projects.origin,projects.owner FROM projects "
	// 		"WHERE (projects.projectid=42)");
	auto projects = select_project_entry(db, 42);
	db.validate();
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(UpdateProjectOwner)
{
	db.expect("UPDATE projects SET owner=42 WHERE (projects.projectid=13)");
	update_project_owner(db, 13, 42);
	db.validate();
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(SelectBook)
{
	db.expect("SELECT books.bookid,books.year,books.title,books.author,"
			"books.description,books.uri,books.directory,books.lang "
			"FROM books "
			"INNER JOIN projects ON (books.bookid=projects.origin) "
			"WHERE (books.bookid=13)");
	select_book(db, *user, 13);
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
		bbuilder.set_owner(*user);
		bbuilder.append(*page);
		book = bbuilder.build();
	}
};

////////////////////////////////////////////////////////////////////////////////
BOOST_FIXTURE_TEST_SUITE(Books, BooksFixture)

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(InsertProject)
{
	db.expect("INSERT INTO projects (origin,pages,owner) VALUES(0,1,42)");
	db.expect("INSERT INTO project_pages (projectid,pageid) VALUES(0,1)");
	auto view = insert_project(db, *book);
	BOOST_CHECK_EQUAL(view, book);
	db.validate();
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(InsertBook)
{
	db.expect("INSERT INTO projects (origin,owner) VALUES(0,42)");
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
BOOST_AUTO_TEST_CASE(UpdateBook)
{
	book->data.author = "new-author";
	book->data.title = "new-title";
	book->data.dir = "new-directory";
	book->data.year = 1917;
	book->data.uri = "new-uri";
	book->data.description = "new-description";
	book->data.lang = "new-language";
	db.expect("UPDATE books SET author='new-author',title='new-title',"
			"directory='new-directory',year=1917,uri='new-uri',"
			"description='new-description',lang='new-language' "
			"WHERE (books.bookid=0)");
	update_book(db, *book);
	db.validate();
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(SelectProject)
{
	db.expect("SELECT project_pages.pageid FROM project_pages "
			"WHERE (project_pages.projectid=42)");
	select_project(db, *book, 42);
	db.validate();
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(SelectProjectIds)
{
	db.expect("SELECT projects.projectid,projects.origin,projects.owner,projects.pages,books.bookid,books.year,books.title,books.author,books.description,books.uri,books.directory,books.lang FROM books INNER JOIN projects ON (books.bookid=projects.origin) WHERE ((projects.owner=42) OR (projects.owner=0))");
	// db.expect("SELECT projects.projectid FROM projects "
	// 		"WHERE ((projects.owner=42) OR (projects.owner=0))");
	select_all_projects(db, *user);
	db.validate();
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_SUITE_END()

