#include <cassert>
#include <mysql_connection.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include "util/ScopeGuard.hpp"
#include "User.hpp"
#include "doc/Book.hpp"
#include "db.hpp"
#include "Sessions.hpp"
#include "Books.hpp"

////////////////////////////////////////////////////////////////////////////////
pcw::Books::Books(SessionPtr session)
	: session_(std::move(session))
{
	assert(session_);
	if (not session_->connection->isValid())
		session_->connection->reconnect();
}

////////////////////////////////////////////////////////////////////////////////
pcw::BookPtr 
pcw::Books::new_book(
	const std::string& title, 
	const std::string& author,
	const std::string& dir
) const {
	static const char *sql1 = "INSERT INTO bookdata "
			 	  "(owner, title, author, directory) VALUES(?,?,?,?)";
	static const char *sql2 = "insert into books "
			 	  "(bookdataid, firstpage, lastpage) VALUES(?,0,0)";

	session_->connection->setAutoCommit(false);
	ScopeGuard sc([this]{
		session_->connection->rollback();
	});

	// insert book data id
	PreparedStatementPtr s{session_->connection->prepareStatement(sql1)};
	assert(s);
	s->setInt(1, session_->user->id);
	s->setString(2, title);
	s->setString(3, author);
	s->setString(4, dir);
	s->executeUpdate();
	const auto dataid = last_insert_id(*s);

	// insert book
	s.reset(session_->connection->prepareStatement(sql2));
	assert(s);
	s->setInt(1, dataid);
	s->executeUpdate();
	const auto bookid = last_insert_id(*s);	

	// commit results
	session_->connection->commit();
	sc.dismiss();
	
	// build book
	const auto book = std::make_shared<Book>();
	book->id = bookid;
	book->data.id = dataid;
	book->data.title = title;
	book->data.path = dir;
	return book;
}

////////////////////////////////////////////////////////////////////////////////
pcw::BookPtr 
pcw::Books::find_book(int bookid) const
{
	static const auto* sql = "SELECT * FROM books NATURAL JOIN bookdata WHERE bookid=?";
	
	// query data;
	PreparedStatementPtr s{session_->connection->prepareStatement(sql)};
	assert(s);
	s->setInt(1, bookid);
	ResultSetPtr res{s->executeQuery()};
	if (not res or not res->next())
		return nullptr;
	
	// we have (at least) one result for the book
	auto book = std::make_shared<Book>();
	book->id = bookid; 
	book->id = res->getInt("bookid");

	// bookdata
	book->data.firstpage = res->getInt("firstpage");
	book->data.lastpage = res->getInt("lastpage");
	book->data.id = res->getInt("bookdataid");
	book->data.owner = res->getInt("owner");
	book->data.year = res->getInt("year");
	book->data.title = res->getString("title");
	book->data.desc = res->getString("description");
	book->data.uri = res->getString("uri");
	book->data.path = res->getString("directory");
	return book;
}

////////////////////////////////////////////////////////////////////////////////
void 
pcw::Books::insert_page(const Book& book, const Page& page) const
{
	static const char *sql = 
		"INSERT INTO linesx ("
		" bookid,"
		" pageid,"
		" lineid,"
		" imagepath,"
		" lstr,"
		" cuts,"
		" lleft,"
		" ltop,"
		" lright,"
		" lbottom)"
		" VALUES(?,?,?,?,?,?,?,?,?,?)";
	session_->connection->setAutoCommit(false);
	ScopeGuard sc([this]{
		session_->connection->rollback();
	});

	// insert lines
	PreparedStatementPtr s;
	for (const auto& line: page) {
		if (not line)
			continue;
		s.reset(session_->connection->prepareStatement(sql));
		assert(s);

		s->setInt(1, book.id);
		s->setInt(2, page.id);
		s->setInt(3, line->id);
		s->setString(4, line->image);
		s->setString(5, line->line());
		s->setString(6, line->cuts_str());
		s->setInt(7, line->box.left);
		s->setInt(8, line->box.top);
		s->setInt(9, line->box.right);
		s->setInt(10, line->box.bottom);
		s->executeUpdate();		
	}

	// commit results
	session_->connection->commit();
	sc.dismiss();
}

////////////////////////////////////////////////////////////////////////////////
bool 
pcw::Books::is_allowed(const Book& book) const
{
	if (is_owner(book))
		return true;
	// TODO: implement this
	return true;
}

////////////////////////////////////////////////////////////////////////////////
bool 
pcw::Books::is_owner(const Book& book) const
{
	return book.data.id == session_->user->id;
}

////////////////////////////////////////////////////////////////////////////////
int 
pcw::Books::last_insert_id(sql::Statement& s)
{
	ResultSetPtr res{s.executeQuery("SELECT last_insert_id()")};
	if (not res or not res->next())
		throw std::runtime_error("(Books) cannot determine id");
	return res->getInt(1);
}

