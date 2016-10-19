#include <crow.h>
#include <cassert>
#include <cppconn/connection.h>
#include <cppconn/prepared_statement.h>
#include "Config.hpp"
#include "Sessions.hpp"
#include "Database.hpp"
#include "User.hpp"
#include "db.hpp"

static const int SHA2_HASH_SIZE = 256;

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
Database::Database(SessionPtr session, ConfigPtr config)
	: scope_guard_()
	, session_(std::move(session))
	, config_(std::move(config))
{
	assert(session_);
	assert(config_);
}

////////////////////////////////////////////////////////////////////////////////
UserPtr 
Database::insert_user(const std::string& name, const std::string& pass) const
{
	static const char *sql = "INSERT INTO users (name, passwd, active) "
				 "VALUES (?, SHA2(?, ?), ?)"
				 ";";
	
	auto conn = connection();
	assert(conn);
	PreparedStatementPtr s(conn->prepareStatement(sql));
	assert(s);
	s->setString(1, name);
	s->setString(2, pass);
	s->setInt(3, SHA2_HASH_SIZE);
	s->setBoolean(4, true);
	s->executeUpdate();

	StatementPtr llid{conn->createStatement()};
	assert(llid);
	ResultSetPtr res{llid->executeQuery("select last_insert_id()")};
	assert(res);
	return res->next() ? 
		std::make_shared<User>(name, "", "", res->getInt(1)) :
		nullptr;
}

////////////////////////////////////////////////////////////////////////////////
UserPtr 
Database::authenticate(const std::string& name, const std::string& pass) const
{
	static const char *sql = "SELECT name,email,institute,userid "
				 "FROM users "
				 "WHERE name = ? and passwd = SHA2(?,?)"
				 ";";
	auto conn = connection();
	assert(conn);
	
	PreparedStatementPtr s(conn->prepareStatement(sql));
	assert(s);
	s->setString(1, name);
	s->setString(2, pass);
	s->setInt(3, SHA2_HASH_SIZE);
	return get_user_from_result_set(ResultSetPtr{s->executeQuery()});
}

////////////////////////////////////////////////////////////////////////////////
UserPtr 
Database::select_user(const std::string& name) const
{
	static const char *sql = "SELECT name,email,institute,userid "
				 "FROM users "
				 "WHERE name = ?"
				 ";";
	auto conn = connection();
	assert(conn);
	PreparedStatementPtr s(conn->prepareStatement(sql));
	assert(s);
	s->setString(1, name);
	return get_user_from_result_set(ResultSetPtr{s->executeQuery()});
}

////////////////////////////////////////////////////////////////////////////////
void 
Database::update_user(const User& user) const
{
	// just update email and institute; not name or userid
	static const char *sql = "UPDATE users "
				 "SET institute=?,email=? "
				 "WHERE userid = ? "
				 ";";
	auto conn = connection();
	assert(conn);
	PreparedStatementPtr s(conn->prepareStatement(sql));
	assert(s);
	s->setString(1, user.institute);
	s->setString(2, user.email);
	s->setInt(3, user.id);
	s->execute();
}

////////////////////////////////////////////////////////////////////////////////
void 
Database::delete_user(const std::string& name) const
{
	static const char *sql = "UPDATE FROM users "
				 "WHERE name = ?"
				 ";";
	auto conn = connection();
	assert(conn);
	PreparedStatementPtr s(conn->prepareStatement(sql));
	assert(s);
	s->setString(1, name);
	s->execute();
}

////////////////////////////////////////////////////////////////////////////////
UserPtr 
Database::get_user_from_result_set(ResultSetPtr res) 
{
	if (not res or not res->next())
		return nullptr;
	return std::make_shared<User>(
		res->getString("name"),
		res->getString("email"),
		res->getString("institute"),
		res->getInt("userid")
	);
}

////////////////////////////////////////////////////////////////////////////////
void 
Database::set_autocommit(bool ac)
{
	if (ac) {
		scope_guard_ = boost::none;
	} else {
		scope_guard_.emplace([this](){
			assert(session_);
			std::lock_guard<std::mutex> lock(session_->mutex);
			session_->connection->rollback();
		});
	}
}

////////////////////////////////////////////////////////////////////////////////
void 
Database::commit()
{
	if (scope_guard_) {
		scope_guard_->dismiss();
	} else {
		CROW_LOG_ERROR << "(Database) call to commit() in auto commit mode";
	}
}

////////////////////////////////////////////////////////////////////////////////
sql::Connection* 
Database::connection() const
{
	assert(config_);
	assert(session_);
	std::lock_guard<std::mutex> lock(session_->mutex);
	sql::Connection* conn = nullptr;

	if (session_->connection and session_->connection->isValid()) {
		conn = session_->connection.get();
	} else if (session_->connection and session_->connection->reconnect()) { 
		conn = session_->connection.get();
	} else { // new connection or could not reconnect
		session_->connection = connect(*config_);
		assert(session_->connection);
		conn = session_->connection.get();
	}
	
	assert(conn);
	// if scope_guard is not set, use autocommit mode
	if (not scope_guard_)
		conn->setAutoCommit(true);
	else 
		conn->setAutoCommit(false);
	return conn;
}
