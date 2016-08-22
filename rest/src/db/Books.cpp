#include <cassert>
#include <mysql_connection.h>
#include <cppconn/prepared_statement.h>
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
	std::lock_guard<std::mutex> lock(session_->mutex);
	if (not session_->connection->isValid())
		session_->connection->reconnect();
}

////////////////////////////////////////////////////////////////////////////////
pcw::BookPtr 
pcw::Books::create_new_book(const std::string& title, const std::string& dir) const
{
	static const char *sql1 = "insert into bookdata "
			 	  "(owner, title, directory) values(?,?,?)";
	static const char *sql2 = "insert into books "
			 	  "(bookdataid, firstpage, lastpage) values(?,0,0)";

	std::lock_guard<std::mutex> lock(session_->mutex);
	session_->connection->setAutoCommit(false);
	ScopeGuard sc([this]{session_->connection->rollback();});

	// insert book data id
	PreparedStatementPtr s{session_->connection->prepareStatement(sql1)};
	assert(s);
	s->setInt(1, session_->user->id);
	s->setString(2, title);
	s->setString(3, dir);
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
int 
pcw::Books::last_insert_id(sql::Statement& s)
{
	ResultSetPtr res{s.executeQuery("select last_insert_id()")};
	if (not res or not res->next())
		throw std::runtime_error("(Books) cannot determine id");
	return res->getInt(1);
}

