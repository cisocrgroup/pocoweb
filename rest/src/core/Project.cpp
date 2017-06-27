#include "utils/Error.hpp"
#include "Book.hpp"
#include "Page.hpp"
#include "Project.hpp"

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
template<class It>
static It
find(It b, It e, int id) noexcept
{
	return std::find_if(b, e, [id](const auto& page) {
		assert(page);
		return page->id() == id;
	});
}

////////////////////////////////////////////////////////////////////////////////
template<class It>
static PagePtr
find_next(It i, It e, size_t n) noexcept
{
	if (static_cast<typename It::difference_type>(n) < std::distance(i, e)) {
		return *(i + n);
	}
	return nullptr;
}

////////////////////////////////////////////////////////////////////////////////
template<class It>
static PagePtr
find_prev(It b, It i, size_t n) noexcept
{
	if (static_cast<typename It::difference_type>(n) <= std::distance(b, i)) {
		return *(i - n);
	}
	return nullptr;
}

////////////////////////////////////////////////////////////////////////////////
template<class It>
static PagePtr
find_page(It b, It e, int id) noexcept
{
	auto i = find(b, e, id);
	return i != e ? *i : nullptr;
}

////////////////////////////////////////////////////////////////////////////////
void
Project::set_id(int id)
{
	if (id_ > 0)
		THROW(Error, "(Project) Cannot reset id ", id_, " to id ", id);
	id_ = id;
}

////////////////////////////////////////////////////////////////////////////////
Project::value_type
Project::find(int pageid) const noexcept
{
	// just do a forward search
	// this should be fast enough
	return find_page(begin(), end(), pageid);
}


////////////////////////////////////////////////////////////////////////////////
Project::value_type
Project::next(int pageid, int val) const noexcept
{
	const auto b = begin();
	const auto e = end();
	const auto i = ::find(b, e, pageid);

	if (i != e) {
		if (val > 0) {
			return find_next(i, e, std::abs(val));
		} else if (val < 0) {
			return find_prev(b, i, std::abs(val));
		} else {
			return *i;
		}
	}
	return nullptr;
}

////////////////////////////////////////////////////////////////////////////////
void
Project::push_back(Page& page)
{
	Base::push_back(page.shared_from_this());
	Base::back()->book_ = std::static_pointer_cast<const Book>(
			origin().shared_from_this());
}

////////////////////////////////////////////////////////////////////////////////
std::ostream&
pcw::operator<<(std::ostream& os, const Project& proj)
{
	if (proj.is_book()) {
		return os << proj.origin().data.author << " <"
			  << proj.origin().data.title << "> ["
			  << proj.origin().id() << "]";
	} else {
		return os << proj.origin() << " {" << proj.id() << "}";
	}
}
