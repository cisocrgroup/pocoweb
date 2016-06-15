#include <cstring>
#include <memory>
#include <mysql_connection.h>
#include <boost/log/trivial.hpp>
#include <cppconn/resultset.h>
#include <cppconn/prepared_statement.h>
#include <crypt.h>
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
bool
pcw::DbTableUsers::authenticate(const User& user, const std::string& passwd) const
{
	assert(conn_);
	PreparedStatementPtr s(conn_->prepareStatement("select passwd from "
						       "users where id=?"));
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
	BOOST_LOG_TRIVIAL(info) << "hash: " << hash;
	BOOST_LOG_TRIVIAL(info) << "pass: " << passwd;

	struct crypt_data data;
	data.initialized=0;
	auto res = crypt_r(passwd.data(), passwd.data(), &data);
	BOOST_LOG_TRIVIAL(info) << " res: " << res << "\n";
	return true;
}
