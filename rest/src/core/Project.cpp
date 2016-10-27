#include "Book.hpp"
#include "Page.hpp"
#include "Project.hpp"

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
template<class It>
static PagePtr 
do_find(It b, It e, int id)
{
	auto i = std::find_if(b, e, [id](const auto& page) {
		assert(page);
		return page->id == id;
	});
	return i != e ? *i : nullptr;
}

////////////////////////////////////////////////////////////////////////////////
void
Project::set_id(int id)
{
	if (id_ > 0)
		throw std::logic_error(
			"(Project) Cannot reset id " +
			std::to_string(id_) + " to id " +
			std::to_string(id)
		);
	id_ = id;
}

////////////////////////////////////////////////////////////////////////////////
Project::value_type 
Project::find(int pageid) const noexcept
{
	// just do a forward search
	// this should be fast enough
	return do_find(begin(), end(), pageid);
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

