#include <cassert>
#include "Book.hpp"
#include "User.hpp"
#include "Page.hpp"
#include "SubProject.hpp"

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
SubProject::SubProject(int id, const User& owner, const Book& book)
	: pages_()
	, owner_(owner.shared_from_this())
	, origin_(std::static_pointer_cast<const Book>(book.shared_from_this()))
	, id_(id)
{
	assert(id_);
}

////////////////////////////////////////////////////////////////////////////////
const Book& 
SubProject::origin() const noexcept
{
	return *origin_;
}

////////////////////////////////////////////////////////////////////////////////
const User& 
SubProject::owner() const noexcept
{
	return *owner_;
}

////////////////////////////////////////////////////////////////////////////////
int 
SubProject::id() const noexcept
{
	return id_;
}

////////////////////////////////////////////////////////////////////////////////
void 
SubProject::each_page(Callback f) const 
{
	for (const auto& page: pages_) {
		assert(page);
		f(*page);
	}
}

////////////////////////////////////////////////////////////////////////////////
void 
SubProject::push_back(Page& page)
{
	pages_.push_back(page.shared_from_this());
}
