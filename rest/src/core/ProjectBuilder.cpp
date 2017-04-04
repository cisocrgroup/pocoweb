#include "Page.hpp"
#include "Book.hpp"
#include "Package.hpp"
#include "BookView.hpp"
#include "ProjectBuilder.hpp"

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
ProjectBuilder::set_book(const Book& book)
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
ProjectBuilder::add_page(int pageid) const
{
	assert(book_);
	assert(project_);
	auto page = book_->find(pageid);
	if (not page)
		throw std::runtime_error("invalid page id: " +
				std::to_string(pageid) + " in book id: " +
				std::to_string(book_->id()));
	project_->push_back(*page);
	return *this;
}

////////////////////////////////////////////////////////////////////////////////
ProjectSptr
ProjectBuilder::build() const
{
	// sort by id and remove equal pages
	std::sort(project_->begin(), project_->end(),
			[](const auto& a, const auto& b) {return a->id() < b->id();});
	auto e = std::unique(project_->begin(), project_->end(),
			[](const auto& a, const auto& b) {return a->id() == b->id();});

	project_->erase(e, project_->end());
	return project_;
}
