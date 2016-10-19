#include <cassert>
#include <boost/log/trivial.hpp>
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
pcw::BookData 
pcw::Books::new_book_data(
	const std::string& title, 
	const std::string& author,
	const std::string& dir
) const {
	static const char *sql = "INSERT INTO bookdata "
			 	  "(owner, title, author, directory) VALUES(?,?,?,?)";

	session_->connection->setAutoCommit(true);

	PreparedStatementPtr s{session_->connection->prepareStatement(sql)};
	assert(s);
	const auto ownerid = session_->user->id;
	s->setInt(1, ownerid);
	s->setString(2, title);
	s->setString(3, author);
	s->setString(4, dir);
	s->executeUpdate();
	const auto dataid = last_insert_id();

	BookData data;
	data.id = dataid;
	data.owner = ownerid;
	data.title = title;
	data.author = author;
	data.path = dir;
	return data;
}

////////////////////////////////////////////////////////////////////////////////
pcw::BookPtr 
pcw::Books::new_book(BookData data) const 
{
	static const char *sql = "INSERT INTO book "
			 	  "(bookdataid,firstpage,lastpage) VALUES(?,0,0)";
	session_->connection->setAutoCommit(true);
	PreparedStatementPtr s{session_->connection->prepareStatement(sql)};
	assert(s);
	s->setInt(1, data.id);
	s->executeUpdate();
	const auto bookid = last_insert_id();
	auto book = std::make_shared<Book>(bookid);
	book->data = std::move(data);
	return book;
}

////////////////////////////////////////////////////////////////////////////////
pcw::BookPtr 
pcw::Books::find_book(int bookid) const
{
	try {
		auto book = std::make_shared<Book>(bookid);
		book->dbload(*session_->connection);
		return book;
	} catch (const std::exception& e) {
		BOOST_LOG_TRIVIAL(error) << e.what();
		return nullptr;
	}
}

////////////////////////////////////////////////////////////////////////////////
void
pcw::Books::insert_book(const Book& book) const
{
	session_->connection->setAutoCommit(false);
	ScopeGuard sc([this]{
		session_->connection->rollback();
	});
	
	book.dbstore(*session_->connection);

	// commit results
	session_->connection->commit();
	sc.dismiss();
}

////////////////////////////////////////////////////////////////////////////////
void 
pcw::Books::insert_page(const Book& book, const Page& page) const
{
	session_->connection->setAutoCommit(false);
	ScopeGuard sc([this]{
		session_->connection->rollback();
	});
	
	page.dbstore(*session_->connection, book.id);

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
pcw::Books::last_insert_id() const
{
	static const char *sql = "SELECT last_insert_id()";
	StatementPtr s{session_->connection->createStatement()};
	assert(s);
	ResultSetPtr res{s->executeQuery(sql)};
	if (not res or not res->next())
		throw std::runtime_error("(Books) cannot determine id");
	return res->getInt(1);
}

