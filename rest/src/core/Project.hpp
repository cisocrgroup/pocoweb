#ifndef pcw_Project_hpp__
#define pcw_Project_hpp__

#include <boost/container/flat_set.hpp>
#include <functional>
#include <memory>

namespace pcw {
	class Book;
	class Page;
	class Project;
	using ProjectPtr = std::shared_ptr<Project>;

	class Project: public std::enable_shared_from_this<Project> {
	public:
		using Callback = std::function<void(Page& page)>;

		virtual ~Project() noexcept = default;
		virtual const Book& origin() const noexcept = 0;
		virtual void each_page(Callback f) const = 0;
		virtual bool is_allowed_for(int id) const noexcept {
			return permissions_.count(id);
		}
		virtual int add_permission(int id) noexcept {
			permissions_.insert(id);
			return id;
		}
		bool is_book() const noexcept {
			return this == static_cast<const void*>(&origin());
		}

	private:
		boost::container::flat_set<int> permissions_;		
	};
}

#endif // pcw_Project_hpp__
