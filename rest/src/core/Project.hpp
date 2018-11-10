#ifndef pcw_Project_hpp__
#define pcw_Project_hpp__

#include <iostream>
#include <memory>
#include <vector>

namespace pcw {
class Book;
class Page;
using PagePtr = std::shared_ptr<Page>;
class Project;
using ProjectSptr = std::shared_ptr<Project>;

class Project
  : private std::vector<PagePtr>
  , public std::enable_shared_from_this<Project>
{
public:
  using Base = std::vector<PagePtr>;
  using value_type = Base::value_type;

  Project(int id = 0)
    : id_(id)
  {}
  virtual ~Project() noexcept = default;
  virtual const Book& origin() const noexcept = 0;
  virtual int owner() const noexcept = 0;
  virtual void set_owner(int) = 0;
  int id() const noexcept { return id_; }
  void set_id(int id);
  bool is_book() const noexcept
  {
    return this == static_cast<const void*>(&origin());
  }
  value_type find(int pageid) const noexcept;
  value_type next(int pageid, int val) const noexcept;
  void push_back(Page& page);
  using Base::back;
  using Base::begin;
  using Base::empty;
  using Base::end;
  using Base::erase;
  using Base::front;
  using Base::size;

private:
  int id_;
};
std::ostream&
operator<<(std::ostream& os, const Project& proj);
}

#endif // pcw_Project_hpp__
