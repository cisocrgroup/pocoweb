#include "User.hpp"
#include "Page.hpp"
#include "Book.hpp"

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
void 
Book::each_page(Callback f) const
{
	for (const auto& page: *this) {
		if (page)
			f(*page);
	}
}

////////////////////////////////////////////////////////////////////////////////
void 
Book::set_owner(const User& user) 
{
	if (owner_)
		throw std::logic_error("(Book) It is not allowed to change the owner of a book");
	owner_ = user.shared_from_this();
}

////////////////////////////////////////////////////////////////////////////////
void
Book::push_back(PagePtr page)
{
	if (page) {
		Base::push_back(std::move(page));
		this->back()->book_ = std::static_pointer_cast<Book>(shared_from_this());
	}
}
