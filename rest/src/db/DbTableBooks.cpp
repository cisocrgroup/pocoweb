#include <sstream>
#include <cstring>
#include <memory>
#include <mysql_connection.h>
#include <boost/log/trivial.hpp>
#include <cppconn/resultset.h>
#include <cppconn/prepared_statement.h>
#include <json.hpp>
#include "util/ScopeGuard.hpp"
#include "db.hpp"
#include "db/Sessions.hpp"
#include "doc/BookData.hpp"
#include "doc/Container.hpp"
#include "doc/Box.hpp"
#include "doc/Line.hpp"
#include "doc/Page.hpp"
#include "doc/Book.hpp"
#include "User.hpp"
#include "DbTableBooks.hpp"

////////////////////////////////////////////////////////////////////////////////
/*
#define JOIN "select * from books "\
	     "natural join bookdata natural join pages "\
	     "join linesx on linesx.bookid = books.bookid "\
	     "and linesx.pageid = pages.pageid "
*/
#define JOIN "select * from linesx natural join pages natural join books natural join bookdata "

////////////////////////////////////////////////////////////////////////////////
pcw::DbTableBooks::DbTableBooks(ConnectionPtr conn)
	: conn_(std::move(conn))
{
	assert(conn_);
}

////////////////////////////////////////////////////////////////////////////////
pcw::BookPtr
pcw::DbTableBooks::get(int bookid, int pageid, int lineid) const
{
	if (bookid > 0 and pageid > 0 and lineid > 0) {
		return getLine(bookid, pageid, lineid);
	} else if (bookid > 0 and pageid > 0) {
		return getPage(bookid, pageid);
	} else if (bookid > 0) {
		return getBook(bookid);
	}
	return nullptr;
}

////////////////////////////////////////////////////////////////////////////////
pcw::BookPtr
pcw::DbTableBooks::getLine(int bookid, int pageid, int lineid) const
{
	assert(bookid > 0 and pageid > 0 and lineid > 0);
	static const char *sql = JOIN
		"where bookid=? and pageid=? and lineid=?";
	conn_->setAutoCommit(true);
	PreparedStatementPtr s{conn_->prepareStatement(sql)};
	assert(s);
	s->setInt(1, bookid);
	s->setInt(2, pageid);
	s->setInt(3, lineid);
	ResultSetPtr res{s->executeQuery()};
	assert(res);
	return makeBook(*res);
}

////////////////////////////////////////////////////////////////////////////////
pcw::BookPtr
pcw::DbTableBooks::getPage(int bookid, int pageid) const
{
	assert(bookid > 0 and pageid > 0);
	assert(conn_);
	static const char *sql = JOIN
		"where bookid=? and pageid=? "
		"order by bookid, pageid, lineid";
	conn_->setAutoCommit(true);
	PreparedStatementPtr s{conn_->prepareStatement(sql)};
	assert(s);
	s->setInt(1, bookid);
	s->setInt(2, pageid);
	ResultSetPtr res{s->executeQuery()};
	assert(res);
	return makeBook(*res);
}

////////////////////////////////////////////////////////////////////////////////
pcw::BookPtr
pcw::DbTableBooks::getBook(int bookid) const
{
	assert(bookid > 0);
	assert(conn_);
	static const char *sql = JOIN
		"where books.bookid = ? "
		"order by bookid, pageid, lineid";
	conn_->setAutoCommit(true);
	PreparedStatementPtr s{conn_->prepareStatement(sql)};
	assert(s);
	s->setInt(1, bookid);
	ResultSetPtr res{s->executeQuery()};
	assert(res);
	return makeBook(*res);
}

////////////////////////////////////////////////////////////////////////////////
pcw::BookPtr
pcw::DbTableBooks::makeBook(sql::ResultSet& res)
{
	BookPtr book = nullptr;
	PagePtr page = nullptr;
	while (res.next()) {
		if (not book)
			book = doMakeBook(res);
		const auto pageid = res.getInt("pageid");
		if (not page) {
			page = doMakePage(res);
			book->push_back(page);
		} else if (page->id != pageid) {
			page = doMakePage(res);
			book->push_back(page);
		}
		page->push_back(doMakeLine(res));
	}
	return book;
}

////////////////////////////////////////////////////////////////////////////////
pcw::BookPtr
pcw::DbTableBooks::doMakeBook(sql::ResultSet& res)
{
	auto book = std::make_shared<Book>();
	book->data.year = res.getInt("year");
	book->id = res.getInt("bookid");
	book->data.id = res.getInt("bookdataid");
	book->data.owner = res.getInt("owner");
	book->data.title = res.getString("title");
	book->data.desc = res.getString("description");
	book->data.author = res.getString("author");
	book->data.uri = res.getString("uri");
	book->data.path = res.getString("directory");
	return book;
}

////////////////////////////////////////////////////////////////////////////////
pcw::PagePtr
pcw::DbTableBooks::doMakePage(sql::ResultSet& res)
{
	auto page = std::make_shared<Page>();
	page->id = res.getInt("pageid");
	page->imagefile = res.getString("imagepath");
	page->ocrfile = res.getString("ocrpath");
	page->box.top = res.getInt("ptop");
	page->box.left = res.getInt("pleft");
	page->box.bottom = res.getInt("pbottom");
	page->box.right = res.getInt("pright");
	return page;
}

////////////////////////////////////////////////////////////////////////////////
pcw::LinePtr
pcw::DbTableBooks::doMakeLine(sql::ResultSet& res)
{
	auto line = std::make_shared<Line>();
	line->id = res.getInt("lineid");
	line->box.top = res.getInt("ltop");
	line->box.left = res.getInt("lleft");
	line->box.bottom = res.getInt("lbottom");
	line->box.right = res.getInt("lright");
	line->line() = res.getString("lstr");
	std::stringstream ios(res.getString("cuts"));
	std::copy(std::istream_iterator<int>(ios),
		  std::istream_iterator<int>(),
		  std::back_inserter(line->cuts()));
	return line;
}

////////////////////////////////////////////////////////////////////////////////
pcw::BookPtr
pcw::DbTableBooks::insertBook(int userid, Book& book) const
{
	// insert all or nothing using scope guard
	assert(conn_);
	conn_->setAutoCommit(false);
	ScopeGuard sc([this]{conn_->rollback();});

	// insert book data and book
	book.data.id = insertBookData(userid, book.data);
	book.id = insertBook(book);
	allowBook(userid, book.id);

	// insert pages
	for (const auto& page: book)
		insertPage(book, *page);

	// done; commit and disable rollback
	conn_->commit();
	sc.dismiss();
	return book.shared_from_this();
}

////////////////////////////////////////////////////////////////////////////////
int
pcw::DbTableBooks::insertBookData(int owner, const BookData& data) const
{

	assert(conn_);
	static const char *sql = "insert into bookdata "
	  		  	 "(owner, title, author, year, "
				 "description, uri, directory) "
				 "values(?,?,?,?,?,?,?)";
	PreparedStatementPtr s{conn_->prepareStatement(sql)};
	assert(s);
	s->setInt(1, owner);
	s->setString(2, data.title);
	s->setString(3, data.author);
	s->setInt(4, data.year);
	s->setString(5, data.desc);
	s->setString(6, data.uri);
	s->setString(7, data.path);
	s->executeUpdate();

	StatementPtr liid{conn_->createStatement()};
	ResultSetPtr res{liid->executeQuery("select last_insert_id()")};
	assert(res);
	if (not res->next())
		THROW("(Database) Cannot determine data id of book");
	return res->getInt(1);
}

////////////////////////////////////////////////////////////////////////////////
int
pcw::DbTableBooks::insertBook(const Book& book) const
{
	assert(conn_);
	static const char *sql = "insert into books "
				 "(bookdataid, firstpage, lastpage) "
				 "values(?,?,?)";
	PreparedStatementPtr s{conn_->prepareStatement(sql)};
	assert(s);
	s->setInt(1, book.data.id);
	s->setInt(2, book.data.firstpage);
	s->setInt(3, book.data.lastpage);
	s->executeUpdate();

	StatementPtr liid{conn_->createStatement()};
	ResultSetPtr res{liid->executeQuery("select last_insert_id()")};
	assert(res);
	if (not res->next())
		THROW(Error, "Cannot determine id of book");
	return res->getInt(1);
}

////////////////////////////////////////////////////////////////////////////////
void
pcw::DbTableBooks::insertPage(const Book& book, const Page& page) const
{
	assert(conn_);
	static const char *sql = "insert into pages "
				 "(bookid, pageid, nlines, imagepath, pleft, pright, ptop, pbottom)"
				 "values (?,?,?,?,?,?,?,?)";
	PreparedStatementPtr s{conn_->prepareStatement(sql)};
	assert(s);
	s->setInt(1, book.id);
	s->setInt(2, page.id);
	s->setInt(3, static_cast<int>(page.size()));
	s->setString(4, page.imagefile.string());
	s->setInt(5, page.box.left);
	s->setInt(6, page.box.right);
	s->setInt(7, page.box.top);
	s->setInt(8, page.box.bottom);
	s->executeUpdate();

	for (const auto& line: page)
		insertLine(book, page, *line);
}

////////////////////////////////////////////////////////////////////////////////
void
pcw::DbTableBooks::insertLine(const Book& book, const Page& page, const Line& line) const
{
	assert(conn_);
	static const char *sql = "insert into linesx "
				 "(bookid, pageid, lineid, lstr, cuts, lleft, lright, ltop, lbottom) "
				 "values (?,?,?,?,?,?,?,?,?)";
	PreparedStatementPtr s{conn_->prepareStatement(sql)};
	assert(s);
	s->setInt(1, book.id);
	s->setInt(2, page.id);
	s->setInt(3, line.id);
	s->setString(4, line.line());
	s->setString(5, cutsToString(line));
	s->setInt(6, line.box.left);
	s->setInt(7, line.box.right);
	s->setInt(8, line.box.top);
	s->setInt(9, line.box.bottom);
	s->executeUpdate();
}

////////////////////////////////////////////////////////////////////////////////
std::vector<pcw::BookPtr>
pcw::DbTableBooks::getAllowedBooks(int userid) const
{
	assert(conn_);
	conn_->setAutoCommit(true);
	//static const char *sql = "select books.bookid,bookdata.bookdataid,bookdata.year,bookdata.owner,bookdata.title,bookdata.author,bookdata.description,bookdata.uri,bookdata.directory from bookpermissions "
	static const char *sql = "select * from bookpermissions "
				 "natural join books natural join bookdata "
				 " where bookpermissions.userid=?";
	PreparedStatementPtr s{conn_->prepareStatement(sql)};
	assert(s);
	s->setInt(1, userid);
	ResultSetPtr res{s->executeQuery()};
	assert(res);

	std::vector<BookPtr> books;
	while (res->next()) {
		books.push_back(doMakeBook(*res));
	}
	return books;
}

////////////////////////////////////////////////////////////////////////////////
void
pcw::DbTableBooks::allowBook(int userid, int bookid) const
{
	assert(conn_);
	conn_->setAutoCommit(true);
	static const char *sql = "insert into bookpermissions "
				 "(userid, bookid) values (?,?)";
	PreparedStatementPtr s{conn_->prepareStatement(sql)};
	assert(s);
	s->setInt(1, userid);
	s->setInt(2, bookid);
	s->executeUpdate();
}

////////////////////////////////////////////////////////////////////////////////
void
pcw::DbTableBooks::denyBook(int userid, int bookid) const
{
	assert(conn_);
	conn_->setAutoCommit(true);
	static const char *sql = "delete from bookpermissions where "
				 "userid=? and bookid=?";
	PreparedStatementPtr s{conn_->prepareStatement(sql)};
	assert(s);
	s->setInt(1, userid);
	s->setInt(2, bookid);
	s->executeUpdate();
}

////////////////////////////////////////////////////////////////////////////////
bool
pcw::DbTableBooks::pageIsAllowed(int userid, int bookid, int pageid) const
{
	assert(conn_);
	conn_->setAutoCommit(true);
	static const char *sql = "select * from bookpermissions left join books "
				 "on bookpermissions.bookid = books.bookid "
				 "where userid=? and bookid=?";

	PreparedStatementPtr s{conn_->prepareStatement(sql)};
	assert(s);
	s->setInt(1, userid);
	s->setInt(2, bookid);
	ResultSetPtr res{s->executeQuery()};
	assert(res);
	while (res->next()) {
		const auto firstpage = res->getInt("firstpage");
		const auto lastpage = res->getInt("lastpage");
		if (firstpage <= pageid and pageid <= lastpage)
			return true;
	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////
std::string
pcw::DbTableBooks::cutsToString(const Line& line)
{
	std::stringstream os;
	std::copy(begin(line.cuts()),
		  end(line.cuts()),
		  std::ostream_iterator<int>(os, " "));
	return os.str();
}

