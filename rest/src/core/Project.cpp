#include "Project.hpp"
#include "Book.hpp"
#include "Page.hpp"
#include "utils/Error.hpp"
#include <crow/logging.h>

using namespace pcw;

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
  const auto it = find_page_id(pageid);
  if (it == end()) {
    return nullptr;
  }
  return *it;
}

////////////////////////////////////////////////////////////////////////////////
Project::value_type
Project::next(int pageid, int val) const noexcept
{
  const auto it = find_page_id(pageid);
  const auto e = end();
  if (it == e) {
    return nullptr;
  }
  if (val > 0) {
    return find_next(it, e, std::abs(val));
  }
  if (val < 0) {
    return find_prev(it, e, std::abs(val));
  }
  return *it;
}

////////////////////////////////////////////////////////////////////////////////
void
Project::push_back(Page& page)
{
  Base::push_back(page.shared_from_this());
  Base::back()->book_ =
    std::static_pointer_cast<const Book>(origin().shared_from_this());
  pageIDs2Index_[Base::back()->id()] = size() - 1;
}

////////////////////////////////////////////////////////////////////////////////
Project::const_iterator
Project::find_page_id(int pageid) const noexcept
{
  const auto offs = pageIDs2Index_.find(pageid);
  if (offs == pageIDs2Index_.end()) {
    return end();
  }
  return begin() + offs->second;
}

////////////////////////////////////////////////////////////////////////////////
void
Project::delete_page(int pageid)
{
  const auto it = pageIDs2Index_.find(pageid);
  if (it == pageIDs2Index_.end()) {
    return;
  }
  Base::erase(begin() + it->second);
  pageIDs2Index_.erase(it);
}

////////////////////////////////////////////////////////////////////////////////
std::ostream&
pcw::operator<<(std::ostream& os, const Project& proj)
{
  if (proj.is_book()) {
    return os << proj.origin().data.author << " <" << proj.origin().data.title
              << "> [" << proj.origin().id() << "]";
  } else {
    return os << proj.origin() << " {" << proj.id() << "}";
  }
}
