#ifndef pcw_BookBuilder_hpp__
#define pcw_BookBuilder_hpp__

#include <boost/filesystem/path.hpp>
#include <memory>
#include <vector>

namespace pcw {
struct BookData;
class Book;
using BookSptr = std::shared_ptr<Book>;
class User;
class Page;
using Path = boost::filesystem::path;

class BookBuilder
{
public:
  BookBuilder();

  BookBuilder& reset();
  const BookBuilder& append(Page& page) const;
  const BookBuilder& append_text(const std::string& str) const;
  const BookBuilder& append_text(const std::wstring& str) const;
  const BookBuilder& set_book_data(BookData data) const;
  const BookBuilder& set_author(std::string author) const;
  const BookBuilder& set_title(std::string title) const;
  const BookBuilder& set_description(std::string desc) const;
  const BookBuilder& set_uri(std::string uri) const;
  const BookBuilder& set_language(std::string lang) const;
  const BookBuilder& set_directory(Path dir) const;
  const BookBuilder& set_year(int year) const;
  const BookBuilder& set_owner(int owner) const;
  const BookBuilder& set_id(int id) const;
  BookSptr build() const;

private:
  bool has_unique_page_ids() const;
  void reorder_pages() const;

  BookSptr book_;
  mutable std::vector<std::shared_ptr<Page>> pages_;
};
}
#endif // pcw_BookBuilder_hpp__
