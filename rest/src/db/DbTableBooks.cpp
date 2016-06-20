#include <sstream>
#include <cstring>
#include <memory>
#include <mysql_connection.h>
#include <boost/log/trivial.hpp>
#include <cppconn/resultset.h>
#include <cppconn/prepared_statement.h>
#include "util/ScopeGuard.hpp"
#include "db.hpp"
#include "doc/Document.hpp"
#include "User.hpp"
#include "DbTableBooks.hpp"

////////////////////////////////////////////////////////////////////////////////
pcw::DbTableBooks::DbTableBooks(ConnectionPtr conn)
	: conn_(std::move(conn))
{
	assert(conn_);
	conn_->setSchema("pocoweb");
}

////////////////////////////////////////////////////////////////////////////////
pcw::PagePtr
pcw::DbTableBooks::getPage(int userid, int bookid, int pageid) const
{
	assert(conn_);
	static const char *sql = "select * from linesx "
				 "where bookid = ? and pageid = ? "
				 "order by lineid";
	conn_->setAutoCommit(true);
	PreparedStatementPtr s{conn_->prepareStatement(sql)};
	assert(s);
	s->setInt(1, bookid);
	s->setInt(2, pageid);	
	auto res = s->executeQuery();

	assert(res);
	auto page = std::make_shared<Page>();
	while (res->next()) {
		page->push_back(std::make_shared<Line>());
		page->back()->id = res->getInt("lineid");
		page->back()->line() = res->getString("line");
		page->back()->box.x0 = res->getInt("x0");
		page->back()->box.y0 = res->getInt("y0");
		page->back()->box.x1 = res->getInt("x1");
		page->back()->box.y1 = res->getInt("y1");
		std::stringstream ios(res->getString("cuts"));
		std::copy(std::istream_iterator<int>(ios),
			  std::istream_iterator<int>(),
			  std::back_inserter(page->back()->cuts()));
	}	
	return page;

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
		throw std::runtime_error("Cannot determine data id of book");
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
		throw std::runtime_error("Cannot determine id of book");
	return res->getInt(1);
}

////////////////////////////////////////////////////////////////////////////////
void
pcw::DbTableBooks::insertPage(const Book& book, const Page& page) const
{
	assert(conn_);
	static const char *sql = "insert into pages "
				 "(bookid, pageid, nlines, imagepath, x0, x1, y0, y1)"
				 "values (?,?,?,?,?,?,?,?)";
	PreparedStatementPtr s{conn_->prepareStatement(sql)};
	assert(s);
	s->setInt(1, book.id);
	s->setInt(2, page.id);
	s->setInt(3, static_cast<int>(page.size()));
	s->setString(4, page.image);
	s->setInt(5, page.box.x0);
	s->setInt(6, page.box.x1);
	s->setInt(7, page.box.y0);
	s->setInt(8, page.box.y1);
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
				 "(bookid, pageid, lineid, line, cuts, x0, x1, y0, y1) "
				 "values (?,?,?,?,?,?,?,?,?)";
	PreparedStatementPtr s{conn_->prepareStatement(sql)};
	assert(s);
	s->setInt(1, book.id);
	s->setInt(2, page.id);
	s->setInt(3, line.id);
	s->setString(4, line.line());
	s->setString(5, cutsToString(line));
	s->setInt(6, line.box.x0);
	s->setInt(7, line.box.x1);
	s->setInt(8, line.box.y0);
	s->setInt(9, line.box.y1);
	s->executeUpdate();
}

////////////////////////////////////////////////////////////////////////////////
std::vector<std::pair<int, pcw::BookData>> 
pcw::DbTableBooks::getAllowedBooks(int userid) const
{
	assert(conn_);
	conn_->setAutoCommit(true);
	static const char *sql = "select * from bookpermissions left join "
				 "(books, bookdata) "
				 "on bookpermissions.bookid = books.bookid and "
				 "books.bookdataid = bookdata.bookdataid "
				 "where bookpermissions.userid=?";
	PreparedStatementPtr s{conn_->prepareStatement(sql)};
	assert(s);
	s->setInt(1, userid);
	ResultSetPtr res{s->executeQuery()};
	assert(res);
	
	std::vector<std::pair<int, BookData>> data;
	//data.reserve(res->getRows());
	while (res->next()) {
		data.emplace_back();
		data.back().first = res->getInt("bookid");
		data.back().second.title = res->getString("title");
		data.back().second.author = res->getString("author");
		data.back().second.desc = res->getString("description");
		data.back().second.year = res->getInt("year");
		data.back().second.uri = res->getString("uri");
	}
	return data;
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

