#include "Book.hpp"
#include <utf8.h>
#include <set>
#include "BookBuilder.hpp"
#include "LineBuilder.hpp"
#include "Page.hpp"
#include "PageBuilder.hpp"

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
BookBuilder::BookBuilder() : book_() { reset(); }

////////////////////////////////////////////////////////////////////////////////
BookBuilder& BookBuilder::reset() {
	book_ = std::make_shared<Book>();
	return *this;
}

////////////////////////////////////////////////////////////////////////////////
const BookBuilder& BookBuilder::append(Page& page) const {
	assert(book_);
	book_->push_back(page);
	assert(not book_->empty());
	// Set page id only if page id = 0
	if (not book_->back()->id_) {
		const auto n = book_->size();
		if (n == 1) {
			book_->back()->id_ = 1;
		} else {
			// assume current_page_id = prev_page_id + 1
			assert(n > 1);
			auto prev_page = *(book_->begin() + (n - 2));
			assert(prev_page);
			book_->back()->id_ = prev_page->id_ + 1;
		}
	}
	book_->back()->book_ = book_;
	return *this;
}

////////////////////////////////////////////////////////////////////////////////
const BookBuilder& BookBuilder::set_book_data(BookData data) const {
	assert(book_);
	book_->data = std::move(data);
	return *this;
}

////////////////////////////////////////////////////////////////////////////////
const BookBuilder& BookBuilder::set_author(std::string author) const {
	assert(book_);
	book_->data.author = std::move(author);
	return *this;
}

////////////////////////////////////////////////////////////////////////////////
const BookBuilder& BookBuilder::set_title(std::string title) const {
	assert(book_);
	book_->data.title = std::move(title);
	return *this;
}

////////////////////////////////////////////////////////////////////////////////
const BookBuilder& BookBuilder::set_description(std::string description) const {
	assert(book_);
	book_->data.description = std::move(description);
	return *this;
}

////////////////////////////////////////////////////////////////////////////////
const BookBuilder& BookBuilder::set_uri(std::string uri) const {
	assert(book_);
	book_->data.uri = std::move(uri);
	return *this;
}

////////////////////////////////////////////////////////////////////////////////
const BookBuilder& BookBuilder::set_language(std::string language) const {
	assert(book_);
	book_->data.lang = std::move(language);
	return *this;
}

////////////////////////////////////////////////////////////////////////////////
const BookBuilder& BookBuilder::set_directory(Path directory) const {
	assert(book_);
	book_->data.dir = std::move(directory);
	return *this;
}

////////////////////////////////////////////////////////////////////////////////
const BookBuilder& BookBuilder::set_year(int year) const {
	assert(book_);
	book_->data.year = year;
	return *this;
}

////////////////////////////////////////////////////////////////////////////////
const BookBuilder& BookBuilder::set_owner(const User& owner) const {
	assert(book_);
	book_->set_owner(owner);
	return *this;
}

////////////////////////////////////////////////////////////////////////////////
const BookBuilder& BookBuilder::set_id(int id) const {
	assert(book_);
	book_->set_id(id);
	return *this;
}

////////////////////////////////////////////////////////////////////////////////
BookSptr BookBuilder::build() const {
	assert(book_);
	// Handle cases where multiple ocr pages have
	// the same page sequence number set in the ocr files.
	if (not has_unique_page_ids(*book_)) {
		reorder_pages(*book_);
	}
	return book_;
}

////////////////////////////////////////////////////////////////////////////////
bool BookBuilder::has_unique_page_ids(const Book& book) {
	std::set<int> ids;
	for (const auto& page : book) {
		if (ids.count(page->id())) return false;
		ids.insert(page->id());
	}
	return true;
}

////////////////////////////////////////////////////////////////////////////////
void BookBuilder::reorder_pages(Book& book) {
	// skip empty books
	if (book.empty()) return;
	static const auto cmp = [](const PagePtr& a, const PagePtr& b) {
		assert(a);
		assert(b);
		return a->ocr.filename().stem() < b->ocr.filename().stem();
	};
	// sort by ocr file name
	std::sort(begin(book), end(book), cmp);
	// set ids from 1 to n
	int id = 1;
	for (auto& page : book) {
		assert(page);
		page->id_ = id++;
	}
}

////////////////////////////////////////////////////////////////////////////////
const BookBuilder& BookBuilder::append_text(const std::string& str) const {
	std::wstring wstr;
	wstr.reserve(str.size());
	utf8::utf8to32(begin(str), end(str), std::back_inserter(wstr));
	return append_text(wstr);
}

////////////////////////////////////////////////////////////////////////////////
const BookBuilder& BookBuilder::append_text(const std::wstring& str) const {
	PageBuilder pbuilder;
	LineBuilder lbuilder;
	auto b = begin(str);
	auto e = end(str);
	do {
		lbuilder.reset();
		auto t = std::find(b, e, L'\n');
		lbuilder.append(std::wstring(b, t), std::distance(b, t), 1);
		pbuilder.append(*lbuilder.build());
		if (t == e) break;
		b = t + 1;
	} while (b != e);
	return append(*pbuilder.build());
}
