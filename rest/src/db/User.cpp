#include <iostream>
#include <memory>
#include <string>
#include "User.hpp"

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
