#ifndef pcw_SubProject_hpp__
#define pcw_SubProject_hpp__

#include <vector>
#include "Project.hpp"

namespace pcw {
	using PagePtr = std::shared_ptr<Page>;

	class SubProject: public Project {
	public:
		SubProject(int id, const User& owner, const Book& book);
		virtual ~SubProject() noexcept override = default;
		virtual const Book& origin() const noexcept override;
		virtual const User& owner() const noexcept override;
		virtual int id() const noexcept override;
		virtual void set_owner(const User& user) override;
		virtual void each_page(Callback f) const override;
	
	private:
		std::vector<PagePtr> pages_;
		std::shared_ptr<const User> owner_;
		std::shared_ptr<const Book> origin_;
		const int id_;
	};
}

#endif // pcw_SubProject_hpp__
