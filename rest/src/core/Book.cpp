#include "Book.hpp"
#include "Page.hpp"
#include "User.hpp"
#include "utils/Error.hpp"

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
void Book::set_owner(const User& user) noexcept {
	if (owner_)
		THROW(Error,
		      "(Book) It is not allowed to change the owner of a book");
	owner_ = user.shared_from_this();
}
