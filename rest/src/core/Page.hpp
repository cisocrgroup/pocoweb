#ifndef pcw_Page_hpp__
#define pcw_Page_hpp__

#include <memory>
#include <vector>

namespace pcw {
	class Book;
	using BookPtr = std::shared_ptr<Book>;
	class Page;
	using PagePtr = std::shared_ptr<Page>;

	class Page: private std::vector<std::string> {
	public:
		using Base = std::vector<std::string>;
		using value_type = Base::value_type;
	
		Page(const Book& book, int i): book(book.shared_from_this()), id(i) {}

		using Base::begin;
		using Base::end;
		using Base::push_back;
		using Base::empty;
		using Base::size;
		
		const std::weak_ptr<const Book> book;
		const int id;
	};
}

#endif // pcw_Page_hpp__
