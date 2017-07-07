#ifndef pcw_Package_hpp__
#define pcw_Package_hpp__

#include <vector>
#include "Project.hpp"

namespace pcw {
using PagePtr = std::shared_ptr<Page>;

class Package : public Project {
       public:
	Package(int id, const User& owner, const Book& book);
	Package(int id = 0);
	virtual ~Package() noexcept override = default;
	void set_origin(const Book& book);
	virtual const Book& origin() const noexcept override;
	virtual const User& owner() const noexcept override;
	virtual void set_owner(const User& owner) override;

       private:
	std::shared_ptr<const User> owner_;
	std::shared_ptr<const Book> origin_;
};
}

#endif  // pcw_Package_hpp__
