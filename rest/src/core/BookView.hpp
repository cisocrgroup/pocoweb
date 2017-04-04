#ifndef pcw_BookView_hpp__
#define pcw_BookView_hpp__

#include <iostream>
#include <memory>
#include <vector>

namespace pcw {
	class Book;
	class Page;
	using PagePtr = std::shared_ptr<Page>;
	class User;
	class BookView;
	using BookViewSptr = std::shared_ptr<BookView>;

	class BookView: private std::vector<PagePtr>,
		        public std::enable_shared_from_this<BookView> {
	public:
		using Base = std::vector<PagePtr>;
		using value_type = Base::value_type;

		BookView(int id = 0): id_(id) {}
		virtual ~BookView() noexcept = default;
		virtual const Book& origin() const noexcept = 0;
		virtual const User& owner() const noexcept = 0;
		int id() const noexcept {return id_;}
		void set_id(int id);
		bool is_book() const noexcept {
			return this == static_cast<const void*>(&origin());
		}
		value_type find(int pageid) const noexcept;
		value_type next(int pageid, int val) const noexcept;
		void push_back(Page& page);
		using Base::erase;
		using Base::back;
		using Base::front;
		using Base::begin;
		using Base::end;
		using Base::empty;
		using Base::size;

	private:
		int id_;
	};
	std::ostream& operator<<(std::ostream& os, const BookView& proj);
}

#endif // pcw_BookView_hpp__
