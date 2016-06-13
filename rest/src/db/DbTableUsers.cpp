#include <cstring>
#include <memory>
#include <mysql_connection.h>
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
	return res->next() ? makeUser(*res) : nullptr;
}

////////////////////////////////////////////////////////////////////////////////
pcw::UserPtr
pcw::DbTableUsers::makeUser(const sql::ResultSet& res)
{
	auto user = std::make_shared<User>(res.getString("name"),
					   res.getString("email"),
					   res.getString("institute"),
					   res.getInt("id"));
	return res.getBoolean("active") ? user : nullptr;
}

////////////////////////////////////////////////////////////////////////////////
bool
pcw::DbTableUsers::authenticate(const std::string& hash,
				    const std::string& passwd)
{
	// $1$salt$...
	std::cout << "  hash: " << hash << "\n";
	std::cout << "passwd: " << passwd << "\n";
	struct crypt_data data;
	//crypt_data data;
	data.initialized=0;
	auto res = crypt_r(passwd.data(), passwd.data(), &data);
	std::cout << "   res: " << res << "\n";

	//data.initialized
	// const auto *hhash = crypt(passwd.data(), "$6$salt$");
	// if (not hhash) {
	// 	std::cout << "ERRNO: " << errno << "\n";
	// 	return false;
	// }
	// std::cout << "   hash: " << hhash << "\n";
	return true;
}
