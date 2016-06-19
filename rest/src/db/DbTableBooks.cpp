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
	assert(conn_);
	static const char *sql = "insert into books "
				 "(owner, title, author, year,"
				 " description, uri, npages) "
				 "values (?,?,?,?,?,?,?)";

	// insert all or nothing using scope guard
	conn_->setAutoCommit(false);
	ScopeGuard sc([this]{conn_->rollback();});

	PreparedStatementPtr s{conn_->prepareStatement(sql)};
	assert(s);
	s->setInt(1, userid);
	s->setString(2, book.title);
	s->setString(3, book.author);
	s->setInt(4, book.year);
	s->setString(5, book.desc);
	s->setString(6, book.uri);	
	s->setInt(7, static_cast<int>(book.size()));
	s->executeUpdate();

	StatementPtr liid{conn_->createStatement()};
	auto res = liid->executeQuery("select last_insert_id()");
	assert(res);
	if (not res->next())
		throw std::runtime_error("Cannot determine id of book");
	book.id = res->getInt(1);
	for (const auto& page: book) 
		insertPage(book, *page);
	
	// done; commit and disable rollback
	conn_->commit();
	sc.dismiss();
	return book.shared_from_this();
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
std::string 
pcw::DbTableBooks::cutsToString(const Line& line)
{
	std::stringstream os;
	std::copy(begin(line.cuts()), 
		  end(line.cuts()),
		  std::ostream_iterator<int>(os, " "));
	return os.str();
}

