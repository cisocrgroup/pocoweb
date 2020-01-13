#include "ProjectBuilder.hpp"
#include "Book.hpp"
#include "Package.hpp"
#include "Page.hpp"
#include "Project.hpp"

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
ProjectBuilder&
ProjectBuilder::reset()
{
  project_ = std::make_shared<Package>();
  assert(project_);
  return *this;
}

////////////////////////////////////////////////////////////////////////////////
ProjectBuilder&
ProjectBuilder::set_origin(const Book& book)
{
  assert(project_);
  book_ = std::static_pointer_cast<const Book>(book.shared_from_this());
  project_->set_origin(*book_);
  project_->set_owner(book.owner());
  assert(book_);
  return *this;
}

////////////////////////////////////////////////////////////////////////////////
const ProjectBuilder&
ProjectBuilder::set_project_id(int pid) const
{
  assert(book_);
  assert(project_);
  project_->set_id(pid);
  assert(project_->id() == pid);
  return *this;
}

////////////////////////////////////////////////////////////////////////////////
const ProjectBuilder&
ProjectBuilder::add_page(int pageid) const
{
  assert(book_);
  assert(project_);
  auto page = book_->find(pageid);
  if (not page) {
    throw std::runtime_error("invalid page id: " + std::to_string(pageid) +
                             " in book id: " + std::to_string(book_->id()));
  }
  // Books must contain only unique page ids (cf. BookBuilder).
  // Insert pages into the set by id
  pages_.insert(page);
  return *this;
}

////////////////////////////////////////////////////////////////////////////////
ProjectSptr
ProjectBuilder::build() const
{
  assert(project_);
  // Add pages in order of their ids.
  for (const auto& page : pages_) {
    project_->push_back(*page);
  }
  return project_;
}

////////////////////////////////////////////////////////////////////////////////
bool
ProjectBuilder::by_id::operator()(const std::shared_ptr<Page>& a,
                                  const std::shared_ptr<Page>& b) const noexcept
{
  return a->id() < b->id();
}
