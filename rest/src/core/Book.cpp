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
Book::push_back(PagePtr page)
{
	if (page) {
		Base::push_back(std::move(page));
		this->back()->book_ = std::static_pointer_cast<Book>(shared_from_this());
	}
}
