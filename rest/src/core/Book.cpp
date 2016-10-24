#include "Page.hpp"
#include "Book.hpp"

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
void
Book::push_back(PagePtr page)
{
	if (page) {
		this->push_back(std::move(page));
		this->back()->book_ = shared_from_this();
	}
}
