#include "BookBuilder.hpp"
#include "Book.hpp"
#include "LineBuilder.hpp"
#include "Page.hpp"
#include "PageBuilder.hpp"
#include <set>
#include <utf8.h>

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
BookBuilder::BookBuilder()
  : book_()
  , pages_()
{
  reset();
}

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
  // automatically assign valid page ids
  if (page.id() == 0) {
    if (pages_.empty()) {
      page.id_ = 1;
    } else {
      // assume current_page_id = prev_page_id + 1
      page.id_ = pages_.back()->id() + 1;
    }
  }
  page.book_ = book_;
  pages_.push_back(page.shared_from_this());
  return *this;
}

////////////////////////////////////////////////////////////////////////////////
const BookBuilder&
BookBuilder::set_book_data(BookData data) const
{
  assert(book_);
  book_->data = std::move(data);
  return *this;
}

////////////////////////////////////////////////////////////////////////////////
const BookBuilder&
BookBuilder::set_author(std::string author) const
{
  assert(book_);
  book_->data.author = std::move(author);
  return *this;
}

////////////////////////////////////////////////////////////////////////////////
const BookBuilder&
BookBuilder::set_title(std::string title) const
{
  assert(book_);
  book_->data.title = std::move(title);
  return *this;
}

////////////////////////////////////////////////////////////////////////////////
const BookBuilder&
BookBuilder::set_description(std::string description) const
{
  assert(book_);
  book_->data.description = std::move(description);
  return *this;
}

////////////////////////////////////////////////////////////////////////////////
const BookBuilder&
BookBuilder::set_uri(std::string uri) const
{
  assert(book_);
  book_->data.uri = std::move(uri);
  return *this;
}

////////////////////////////////////////////////////////////////////////////////
const BookBuilder&
BookBuilder::set_language(std::string language) const
{
  assert(book_);
  book_->data.lang = std::move(language);
  return *this;
}

////////////////////////////////////////////////////////////////////////////////
const BookBuilder&
BookBuilder::set_directory(Path directory) const
{
  assert(book_);
  book_->data.dir = std::move(directory);
  return *this;
}

////////////////////////////////////////////////////////////////////////////////
const BookBuilder&
BookBuilder::set_year(int year) const
{
  assert(book_);
  book_->data.year = year;
  return *this;
}

////////////////////////////////////////////////////////////////////////////////
const BookBuilder&
BookBuilder::set_owner(int owner) const
{
  assert(book_);
  book_->set_owner(owner);
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
  // Handle cases where multiple ocr pages have
  // the same page sequence number set in the ocr files.
  if (not has_unique_page_ids()) {
    reorder_pages();
  }
  // Add pages to the book.
  for (auto& page : pages_) {
    book_->push_back(*page);
  }
  return book_;
}

////////////////////////////////////////////////////////////////////////////////
bool
BookBuilder::has_unique_page_ids() const
{
  std::set<int> ids;
  for (const auto& page : pages_) {
    if (ids.count(page->id())) {
      return false;
    }
    ids.insert(page->id());
  }
  return true;
}

////////////////////////////////////////////////////////////////////////////////
void
BookBuilder::reorder_pages() const
{
  // skip empty books
  if (pages_.empty())
    return;
  static const auto cmp = [](const PagePtr& a, const PagePtr& b) {
    assert(a);
    assert(b);
    return a->ocr.filename().stem() < b->ocr.filename().stem();
  };
  // sort by ocr file name
  std::sort(begin(pages_), end(pages_), cmp);
  // set ids from 1 to n
  int id = 1;
  for (auto& page : pages_) {
    assert(page);
    page->id_ = id++;
  }
}

////////////////////////////////////////////////////////////////////////////////
const BookBuilder&
BookBuilder::append_text(const std::string& str) const
{
  std::wstring wstr;
  wstr.reserve(str.size());
  utf8::utf8to32(begin(str), end(str), std::back_inserter(wstr));
  return append_text(wstr);
}

////////////////////////////////////////////////////////////////////////////////
const BookBuilder&
BookBuilder::append_text(const std::wstring& str) const
{
  PageBuilder pbuilder;
  LineBuilder lbuilder;
  auto b = begin(str);
  auto e = end(str);
  do {
    lbuilder.reset();
    auto t = std::find(b, e, L'\n');
    lbuilder.append(std::wstring(b, t), std::distance(b, t), 1);
    pbuilder.append(*lbuilder.build());
    if (t == e)
      break;
    b = t + 1;
  } while (b != e);
  return append(*pbuilder.build());
}
