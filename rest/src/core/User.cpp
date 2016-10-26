#include <cassert>
#include <iostream>
#include <memory>
#include <string>
#include <cppconn/resultset.h>
#include <cppconn/connection.h>
#include <cppconn/prepared_statement.h>
#include "Project.hpp"
#include "User.hpp"

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
bool
User::has_permission(const Project& project) const noexcept
{
	return project.is_allowed_for(id);
}

////////////////////////////////////////////////////////////////////////////////
std::ostream&
pcw::operator<<(std::ostream& os, const User& user)
{
	return os << user.name << " <" << user.email << ">";
}
