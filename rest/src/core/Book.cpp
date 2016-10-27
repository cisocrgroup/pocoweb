#include "User.hpp"
#include "Page.hpp"
#include "Book.hpp"

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
void 
Book::set_owner(const User& user) 
{
	if (owner_)
		throw std::logic_error("(Book) It is not allowed to change the owner of a book");
	owner_ = user.shared_from_this();
}

