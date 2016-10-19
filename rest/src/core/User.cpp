#include <cassert>
#include <iostream>
#include <memory>
#include <string>
#include <cppconn/resultset.h>
#include <cppconn/connection.h>
#include <cppconn/prepared_statement.h>
#include "util.hpp"
#include "db.hpp"
#include "User.hpp"

////////////////////////////////////////////////////////////////////////////////
pcw::UserPtr
pcw::User::create(sql::Connection& conn, const std::string& n)
{
	static const char *sql = "SELECT name, email, institute, userid, active "
				 "FROM users WHERE name=? or email=?";
	PreparedStatementPtr s{conn.prepareStatement(sql)};
	assert(s);
	s->setString(1, n);
	s->setString(2, n);
	ResultSetPtr res{s->executeQuery()};
	assert(res);
	return res->next() ? create(*res) : nullptr;
}

////////////////////////////////////////////////////////////////////////////////
pcw::UserPtr
pcw::User::create(const sql::ResultSet& res)
{
	auto user = std::make_shared<User>(res.getString("name"),
					   res.getString("email"),
					   res.getString("institute"),
					   res.getInt("userid"));
	return res.getBoolean("active") ? user : nullptr;
}

////////////////////////////////////////////////////////////////////////////////
pcw::User::User(std::string n,
		std::string e,
		std::string i,
		int iid)
	: name(std::move(n))
	, email(std::move(e))
	, institute(std::move(i))
	, id(iid)
{
}

////////////////////////////////////////////////////////////////////////////////
void
pcw::User::store(sql::Connection& conn, const std::string& passwd, bool active) const
{
	static const char *sql = "INSERT INTO users "
				 "(name, email, insititute, userid, passwd, active) "
				 "values (?,?,?,?,?,?)";
	PreparedStatementPtr s{conn.prepareStatement(sql)};
	assert(s);
	s->setString(1, name);
	s->setString(2, email);
	s->setString(3, institute);
	s->setInt(4, id);
	auto salt = pcw::gensalt();
	auto ssum = pcw::genhash(salt, passwd);
	auto hash = salt + '$' + ssum;
	s->setString(5, hash);
	s->setBoolean(6, active);
	s->executeUpdate();
}

////////////////////////////////////////////////////////////////////////////////
bool
pcw::User::authenticate(sql::Connection& conn, const std::string& passwd) const
{
	static const char *sql = "SELECT userid FROM users"
				 "       WHERE userid=? AND"
				 "             passwd=PASSWORD(?)";
	PreparedStatementPtr s{conn.prepareStatement(sql)};
	assert(s);
	s->setInt(1, id);
	s->setString(2, passwd);
	ResultSetPtr res{s->executeQuery()};
	return res and res->next();
	// return res and res->next() ? true : false;check(res->getString(1), passwd) : false;
}

// ////////////////////////////////////////////////////////////////////////////////
// nlohmann::json
// pcw::User::json() const
// {
// 	nlohmann::json j;
// 	j["name"] = name;
// 	j["email"] = email;
// 	j["institute"] = institute;
// 	j["id"] = id;
// 	return j;
// }

////////////////////////////////////////////////////////////////////////////////
std::ostream&
pcw::operator<<(std::ostream& os, const User& user)
{
	return os << user.name << " <" << user.email << ">";
}
