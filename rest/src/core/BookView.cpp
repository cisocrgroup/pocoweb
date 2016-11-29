#include "Error.hpp"
#include "Book.hpp"
#include "Page.hpp"
#include "BookView.hpp"

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
template<class It> static It
find(It b, It e, int id) noexcept
{
	return std::find_if(b, e, [id](const auto& page) {
		assert(page);
		return page->id() == id;
	});
}

////////////////////////////////////////////////////////////////////////////////
template<class It> static PagePtr
find_page(It b, It e, int id) noexcept
{
	auto i = find(b, e, id);
	return i != e ? *i : nullptr;
}

////////////////////////////////////////////////////////////////////////////////
void
BookView::set_id(int id)
{
	if (id_ > 0)
		THROW(Error, "(BookView) Cannot reset id ", id_, " to id ", id);
	id_ = id;
}

////////////////////////////////////////////////////////////////////////////////
BookView::value_type
BookView::find(int pageid) const noexcept
{
	// just do a forward search
	// this should be fast enough
	return find_page(begin(), end(), pageid);
}


////////////////////////////////////////////////////////////////////////////////
BookView::value_type
BookView::next(int pageid, int val) const noexcept
{
	const auto b = begin();
	const auto e = end();
	const auto i = ::find(b, e, pageid);
	const auto amount = static_cast<size_t>(std::abs(val));

	if (i != e) {
		if (val < 0 and amount <= static_cast<size_t>(std::distance(b, i))) {
			return *(std::prev(i, amount));
		} else if (0 <= val and amount <= static_cast<size_t>(std::distance(i, e))) {
			return *(std::next(i, amount));
		}
	}
	return nullptr;
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
