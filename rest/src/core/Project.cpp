#include "Book.hpp"
#include "Page.hpp"
#include "Project.hpp"

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
Project::value_type 
Project::find(int pageid) const noexcept
{
	return nullptr;
}

////////////////////////////////////////////////////////////////////////////////
void
Project::push_back(PagePtr page)
{
	if (not page)
		return;
	Base::push_back(std::move(page));
	Base::back()->book_ = 
		std::static_pointer_cast<const Book>(origin().shared_from_this());
}

