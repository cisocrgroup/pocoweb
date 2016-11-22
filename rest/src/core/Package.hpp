#ifndef pcw_Package_hpp__
#define pcw_Package_hpp__

#include <vector>
#include "BookView.hpp"

namespace pcw {
	using PagePtr = std::shared_ptr<Page>;

	class Package: public BookView {
	public:
		Package(int id, const User& owner, const Book& book);
		virtual ~Package() noexcept override = default;
		virtual const Book& origin() const noexcept override;
		virtual const User& owner() const noexcept override;

	private:
		const std::shared_ptr<const User> owner_;
		const std::shared_ptr<const Book> origin_;
	};
}

#endif // pcw_Package_hpp__
