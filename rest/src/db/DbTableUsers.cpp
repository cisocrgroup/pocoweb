#include <cstring>
#include <memory>
#include <mysql_connection.h>
#include <boost/log/trivial.hpp>
#include <cppconn/resultset.h>
#include <cppconn/prepared_statement.h>
#include "util/hash.hpp"
#include "db.hpp"
#include "User.hpp"
#include "DbTableUsers.hpp"

////////////////////////////////////////////////////////////////////////////////
pcw::DbTableUsers::DbTableUsers(ConnectionPtr conn)
	: conn_(std::move(conn))
{
	assert(conn_);
	conn_->setSchema("pocoweb");
}

////////////////////////////////////////////////////////////////////////////////
pcw::UserPtr
pcw::DbTableUsers::findUserById(int id) const
{
	assert(conn_);
	PreparedStatementPtr s{conn_->prepareStatement("select * from users "
						       "where userid=?")};
	assert(s);
	s->setInt(1, id);
	ResultSetPtr res{s->executeQuery()};
	assert(res);
	return res->next() ? User::create(*res) : nullptr;
}

////////////////////////////////////////////////////////////////////////////////
pcw::UserPtr
pcw::DbTableUsers::findUserByEmail(const std::string& email) const
{
	assert(conn_);
	PreparedStatementPtr s(conn_->prepareStatement("select * from users "
						       "where email=?"));
	assert(s);
	s->setString(1, email);
	ResultSetPtr res(s->executeQuery());
	assert(res);
	return res->next() ? User::create(*res) : nullptr;
}

////////////////////////////////////////////////////////////////////////////////
pcw::UserPtr
pcw::DbTableUsers::findUserByName(const std::string& name) const
{
	assert(conn_);
	PreparedStatementPtr s(conn_->prepareStatement("select * from users "
						       "where name=?"));
	assert(s);
	s->setString(1, name);
	ResultSetPtr res(s->executeQuery());
	assert(res);
	return res->next() ? User::create(*res) : nullptr;
}

////////////////////////////////////////////////////////////////////////////////
pcw::UserPtr 
pcw::DbTableUsers::findUserByNameOrEmail(const std::string& what) const
{
	auto user = findUserByName(what);
	return user? user : findUserByEmail(what);
}

////////////////////////////////////////////////////////////////////////////////
pcw::UserPtr
pcw::DbTableUsers::createUser(const std::string& name,
			      const std::string& email,
			      const std::string& institute,
			      const std::string& password) const
{
	auto salt = pcw::gensalt();
	auto ssum = pcw::genhash(salt, password);
	auto hash = salt + '$' + ssum;

	assert(conn_);
	conn_->setAutoCommit(true);
	PreparedStatementPtr s(conn_->prepareStatement("insert into users "
						       "(name, email, institute, passwd, active) "
						       " values (?, ?, ?, ?, true)"));
	assert(s);
	s->setString(1, name);
	s->setString(2, email);
	s->setString(3, institute);
	s->setString(4, hash);
	s->executeUpdate();

	StatementPtr liid{conn_->createStatement()};
	ResultSetPtr res{liid->executeQuery("select last_insert_id()")};
	assert(res);
	if (not res->next())
		throw std::runtime_error("Could not determine user id");
	return findUserById(res->getInt(1));
}

////////////////////////////////////////////////////////////////////////////////
bool
pcw::DbTableUsers::authenticate(const User& user, const std::string& passwd) const
{
	assert(conn_);
	PreparedStatementPtr s(conn_->prepareStatement("select passwd from "
						       "users where userid=?"));
	assert(s);
	s->setInt(1, user.id);
	ResultSetPtr res(s->executeQuery());
	assert(res);
	return res->next() ? authenticate(res->getString(1), passwd) : false;
}

////////////////////////////////////////////////////////////////////////////////
bool
pcw::DbTableUsers::authenticate(const std::string& hash,
				const std::string& passwd)
{
	// $1$salt$...
	return pcw::check(hash, passwd);
}
