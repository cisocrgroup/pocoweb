#include <iostream>
#include <memory>
#include <string>
#include <cppconn/resultset.h>
#include "User.hpp"

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
std::ostream&
pcw::operator<<(std::ostream& os, const User& user)
{
	return os << user.name << " <" << user.email << ">";
}
