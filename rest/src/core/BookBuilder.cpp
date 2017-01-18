#include "Page.hpp"
#include "Book.hpp"
#include "BookBuilder.hpp"

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
BookBuilder&
BookBuilder::reset()
{
	book_ = std::make_shared<Book>();
	return *this;
}

////////////////////////////////////////////////////////////////////////////////
const BookBuilder&
BookBuilder::append(Page& page) const
{
	assert(book_);
	book_->push_back(page);
	assert(not book_->empty());
	book_->back()->id_ = static_cast<int>(book_->size());
	book_->back()->book_ = book_;
	return *this;
}

////////////////////////////////////////////////////////////////////////////////
const BookBuilder&
BookBuilder::set_author(std::string author) const
{
	assert(book_);
	book_->author = std::move(author);
	return *this;
}

////////////////////////////////////////////////////////////////////////////////
const BookBuilder&
BookBuilder::set_title(std::string title) const
{
	assert(book_);
	book_->title = std::move(title);
	return *this;
}

////////////////////////////////////////////////////////////////////////////////
const BookBuilder&
BookBuilder::set_description(std::string description) const
{
	assert(book_);
	book_->description = std::move(description);
	return *this;
}

////////////////////////////////////////////////////////////////////////////////
const BookBuilder&
BookBuilder::set_uri(std::string uri) const
{
	assert(book_);
	book_->uri = std::move(uri);
	return *this;
}

////////////////////////////////////////////////////////////////////////////////
const BookBuilder&
BookBuilder::set_language(std::string language) const
{
	assert(book_);
	book_->lang = std::move(language);
	return *this;
}

////////////////////////////////////////////////////////////////////////////////
const BookBuilder&
BookBuilder::set_directory(Path directory) const
{
	assert(book_);
	book_->dir = std::move(directory);
	return *this;
}

////////////////////////////////////////////////////////////////////////////////
const BookBuilder&
BookBuilder::set_year(int year) const
{
	assert(book_);
	book_->year = year;
	return *this;
}

////////////////////////////////////////////////////////////////////////////////
const BookBuilder&
BookBuilder::set_owner(UserSptr owner) const
{
	assert(book_);
	if (owner)
		book_->set_owner(*owner);
	return *this;
}

////////////////////////////////////////////////////////////////////////////////
const BookBuilder&
BookBuilder::set_id(int id) const
{
	assert(book_);
	book_->set_id(id);
	return *this;
}

////////////////////////////////////////////////////////////////////////////////
BookSptr
BookBuilder::build() const
{
	assert(book_);
	return book_;
}
