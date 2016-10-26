#ifndef pcw_Project_hpp__
#define pcw_Project_hpp__

#include <functional>
#include <memory>

namespace pcw {
	class Book;
	class Page;
	class User;
	class Project;
	using ProjectPtr = std::shared_ptr<Project>;

	class Project: public std::enable_shared_from_this<Project> {
	public:
		using Callback = std::function<void(Page& page)>;

		virtual ~Project() noexcept = default;
		virtual const Book& origin() const noexcept = 0;
		virtual const User& owner() const noexcept = 0;
		virtual int id() const noexcept = 0;
		virtual void each_page(Callback f) const = 0;
		bool is_book() const noexcept {
			return this == static_cast<const void*>(&origin());
		}
	};
}

#endif // pcw_Project_hpp__
