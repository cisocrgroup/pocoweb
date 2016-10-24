#include "Page.hpp"
#include "Book.hpp"

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
void
Book::push_back(PagePtr page)
{
	page->book = shared_from_this();
	this->push_back(std::move(page));
}
