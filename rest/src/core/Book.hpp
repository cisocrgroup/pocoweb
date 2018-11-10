#ifndef pcw_Book_hpp__
#define pcw_Book_hpp__

#include "Project.hpp"
#include <boost/filesystem/path.hpp>
#include <memory>
#include <vector>

namespace pcw {
class Book;
using BookPtr = std::shared_ptr<Book>;
using BookSptr = std::shared_ptr<Book>;
using Path = boost::filesystem::path;

struct BookData
{
  BookData()
    : author()
    , title()
    , description()
    , uri()
    , profilerUrl("local")
    , lang()
    , dir()
    , year()
  {}
  std::string author, title, description, uri, profilerUrl, lang;
  Path dir;
  int year;
};

class Book : public Project
{
public:
  Book(int id = 0)
    : Project(id)
    , data()
    , owner_()
  {}
  Book(const Book& other) = delete;
  Book& operator=(const Book& other) = delete;
  Book(Book&& other) = delete;
  Book& operator=(Book&& other) = delete;
  virtual ~Book() noexcept override = default;
  virtual const Book& origin() const noexcept override { return *this; }
  virtual int owner() const noexcept override { return owner_; }
  virtual void set_owner(int owner) override;
  std::shared_ptr<Book> book_ptr()
  {
    return std::dynamic_pointer_cast<Book>(shared_from_this());
  }
  std::shared_ptr<const Book> book_ptr() const
  {
    return std::dynamic_pointer_cast<const Book>(shared_from_this());
  }

  BookData data;

private:
  int owner_;
};
}

#endif // pcw_Book_hpp__
