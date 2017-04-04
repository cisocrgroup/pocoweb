#include <cassert>
#include <iostream>
#include <memory>
#include <string>
#include "Project.hpp"
#include "User.hpp"

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
User::User(std::string n, const std::string& p, std::string e, std::string i, int id)
	: User(std::move(n), Password::make(p), std::move(e), std::move(i), id)
{}

////////////////////////////////////////////////////////////////////////////////
User::User(std::string n, Password p, std::string e, std::string i, int id)
	: name(std::move(n))
	, password(std::move(p))
	, email(std::move(e))
	, institute(std::move(i))
	, id_(id)
{}

////////////////////////////////////////////////////////////////////////////////
std::ostream&
pcw::operator<<(std::ostream& os, const User& user)
{
	return os << user.name
		  << " <" << user.email << "> ["
		  << user.id() << "] {"
		  << user.institute << "}";
}
