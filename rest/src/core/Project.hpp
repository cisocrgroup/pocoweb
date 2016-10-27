#ifndef pcw_Project_hpp__
#define pcw_Project_hpp__

#include <functional>
#include <memory>
#include <vector>

namespace pcw {
	class Book;
	class Page;
	using PagePtr = std::shared_ptr<Page>;
	class User;
	class Project;
	using ProjectPtr = std::shared_ptr<Project>;

	class Project: private std::vector<PagePtr>,
		       public std::enable_shared_from_this<Project> {
	public:
		using Base = std::vector<PagePtr>;
		using value_type = Base::value_type;
	
		virtual ~Project() noexcept = default;
		virtual const Book& origin() const noexcept = 0;
		virtual const User& owner() const noexcept = 0;
		virtual int id() const noexcept = 0;
		bool is_book() const noexcept {
			return this == static_cast<const void*>(&origin());
		}
		value_type find(int pageid) const noexcept;
		void push_back(PagePtr page);
		using Base::back;
		using Base::front;
		using Base::begin;
		using Base::end;
		using Base::empty;
		using Base::size;
	};
}

#endif // pcw_Project_hpp__
