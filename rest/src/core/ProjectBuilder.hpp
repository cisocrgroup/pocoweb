#ifndef pcw_ProjectBuilder_hpp__
#define pcw_ProjectBuilder_hpp__

#include <boost/filesystem/path.hpp>
#include <memory>
#include <set>

namespace pcw {
class Page;
class Project;
using ProjectSptr = std::shared_ptr<Project>;
class Book;
using ConstBookSptr = std::shared_ptr<const Book>;
class Package;
using PackageSptr = std::shared_ptr<Package>;

class ProjectBuilder
{
public:
  ProjectBuilder()
    : project_()
    , book_()
  {
    reset();
  }

  ProjectBuilder& reset();
  ProjectBuilder& set_origin(const Book& book);
  const ProjectBuilder& set_project_id(int id) const;
  const ProjectBuilder& add_page(int pageid) const;
  ProjectSptr build() const;

private:
  struct by_id
  {
    bool operator()(const std::shared_ptr<Page>&,
                    const std::shared_ptr<Page>&) const noexcept;
  };
  mutable std::set<std::shared_ptr<Page>, by_id> pages_;
  PackageSptr project_;
  ConstBookSptr book_;
};
}
#endif // pcw_ProjectBuilder_hpp__
