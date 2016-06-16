#include <cstring>
#include <memory>
#include <mysql_connection.h>
#include <boost/log/trivial.hpp>
#include <cppconn/resultset.h>
#include <cppconn/prepared_statement.h>
#include "util/hash.hpp"
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
pcw::BookPtr
pcw::DbTableBooks::insertBook(const User& owner, Book& book) const
{
	assert(conn_);
	PreparedStatementPtr s{
		conn_->prepareStatement("insert into books "
				        "(owner, title, author, year, description, uri)"
				        " values (?, ?, ?, ?, ?, ?)")
	};
	assert(s);
	s->setInt(1, owner.id);
	s->setString(2, book.title);
	s->setString(3, book.author);
	s->setInt(4, book.year);
	s->setString(5, book.desc);
	s->setString(6, book.uri);	
	s->executeUpdate();

	StatementPtr lid{conn_->createStatement()};
	auto res = lid->executeQuery("select last_insert_id()");
	assert(res);
	if (res->next()) {
		book.id = res->getInt(1);
	}
	return book.shared_from_this();
}

