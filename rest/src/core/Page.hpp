#ifndef pcw_Page_hpp__
#define pcw_Page_hpp__

#include "Book.hpp"
#include "Box.hpp"
#include "Line.hpp"
#include "util.hpp"
#include <boost/filesystem/path.hpp>
#include <memory>
#include <vector>

namespace pcw {
class Project;
class Book;
class Page;
using PageSptr = std::shared_ptr<Page>;

class Page
  : private std::vector<LinePtr>
  , public std::enable_shared_from_this<Page>
{
public:
  using Path = boost::filesystem::path;
  using Base = std::vector<LinePtr>;
  using value_type = Base::value_type;

  Page(int id = 0, Box b = {})
    : box(b)
    , ocr()
    , img()
    , file_type(FileType::Other)
    , book_()
    , id_(id)
  {
  }

  using Base::back;
  using Base::begin;
  using Base::empty;
  using Base::end;
  using Base::front;
  using Base::size;
  const Book& book() const noexcept { return *book_.lock(); }

  void push_back(LinePtr line)
  {
    Base::push_back(std::move(line));
    if (!this->back()) {
      return;
    }
    this->back()->page_ = shared_from_this();
    this->back()->id_ = static_cast<int>(this->size());
  }
  Line& operator[](int id) noexcept { return *Base::operator[](id - 1); }
  const Line& operator[](int id) const noexcept
  {
    return *Base::operator[](id - 1);
  }
  LinePtr find(int id) const noexcept
  {
    if (id > 0 and id <= static_cast<int>(this->size()))
      return Base::operator[](id - 1);
    return nullptr;
  }
  void insert_line(LinePtr line) { Base::operator[](line->id() - 1) = line; }
  void delete_line(int id);
  bool has_ocr_path() const noexcept { return not ocr.empty(); }
  bool has_img_path() const noexcept { return not img.empty(); }
  bool contains(int id) const noexcept
  {
    return static_cast<size_t>(id) < this->size() and id > 0;
  }
  int id() const noexcept { return id_; }
  void set_id(int id) noexcept { id_ = id; }

  Box box;
  Path ocr, img;
  FileType file_type;

private:
  std::weak_ptr<const Book> book_;
  int id_;
  friend class Project;
  friend class PageBuilder;
  friend class BookBuilder;
};
}

#endif // pcw_Page_hpp__
