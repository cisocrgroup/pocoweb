#ifndef pcw_Package_hpp__
#define pcw_Package_hpp__

#include "Project.hpp"
#include <vector>

namespace pcw {
using PagePtr = std::shared_ptr<Page>;

class Package : public Project
{
public:
  Package(int id, int owner, const Book& book);
  Package(int id = 0);
  virtual ~Package() noexcept override = default;
  void set_origin(const Book& book);
  virtual const Book& origin() const noexcept override;
  virtual int owner() const noexcept override;
  virtual void set_owner(int owner) override;

private:
  int owner_;
  std::shared_ptr<const Book> origin_;
};
}

#endif // pcw_Package_hpp__
