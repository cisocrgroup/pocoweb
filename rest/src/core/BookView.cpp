#include "Book.hpp"
#include "Page.hpp"
#include "BookView.hpp"

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
template<class It>
static PagePtr
do_find(It b, It e, int id)
{
	auto i = std::find_if(b, e, [id](const auto& page) {
		assert(page);
		return page->id() == id;
	});
	return i != e ? *i : nullptr;
}

////////////////////////////////////////////////////////////////////////////////
void
BookView::set_id(int id)
{
	if (id_ > 0)
		throw std::logic_error(
			"(BookView) Cannot reset id " +
			std::to_string(id_) + " to id " +
			std::to_string(id)
		);
	id_ = id;
}

////////////////////////////////////////////////////////////////////////////////
BookView::value_type
BookView::find(int pageid) const noexcept
{
	// just do a forward search
	// this should be fast enough
	return do_find(begin(), end(), pageid);
}

////////////////////////////////////////////////////////////////////////////////
void
BookView::push_back(PagePtr page)
{
	if (not page)
		return;
	Base::push_back(std::move(page));
	Base::back()->book_ =
		std::static_pointer_cast<const Book>(origin().shared_from_this());
}

////////////////////////////////////////////////////////////////////////////////
std::ostream&
pcw::operator<<(std::ostream& os, const BookView& proj)
{
	if (proj.is_book()) {
		return os << proj.origin().author << " <"
			  << proj.origin().title << "> ["
			  << proj.origin().id() << "]";
	} else {
		return os << proj.origin() << " {" << proj.id() << "}";
	}
}
