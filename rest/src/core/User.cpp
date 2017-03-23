#include <cassert>
#include <iostream>
#include <memory>
#include <string>
#include "BookView.hpp"
#include "User.hpp"

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
bool
User::has_permission(const BookView& project) const noexcept
{
	return project.owner().id() == id_;
}

////////////////////////////////////////////////////////////////////////////////
std::ostream&
pcw::operator<<(std::ostream& os, const User& user)
{
	return os << user.name
		  << " <" << user.email << "> ["
		  << user.id() << "] {"
		  << user.institute << "}";
}
