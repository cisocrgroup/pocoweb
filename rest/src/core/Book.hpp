#ifndef pcw_Book_hpp__
#define pcw_Book_hpp__

#include <memory>
#include <vector>

namespace pcw {
	class User;
	using UserPtr = std::shared_ptr<User>;
	class Book;
	using BookPtr = std::shared_ptr<Book>;
	class Page;
	using PagePtr = std::shared_ptr<Page>;

	class Book: private std::vector<PagePtr>,
		    public std::enable_shared_from_this<Book> {
	public:
		using Base = std::vector<PagePtr>;
		using value_type = Base::value_type;
	
		Book(UserPtr o, int i): owner(std::move(o)), id(i) {}

		using Base::begin;
		using Base::end;
		using Base::push_back;
		using Base::empty;
		using Base::size;
		
		const UserPtr owner;
		std::string author, title;
		const int id;
		int year;
	};
}

#endif // pcw_Book_hpp__
