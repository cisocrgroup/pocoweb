#ifndef pcw_Page_hpp__
#define pcw_Page_hpp__

#include <boost/filesystem/path.hpp>
#include <memory>
#include <vector>
#include "Box.hpp"
#include "Book.hpp"
#include "Line.hpp"

namespace pcw {
	class Book;
	using BookPtr = std::shared_ptr<Book>;
	class Page;
	using PagePtr = std::shared_ptr<Page>;

	class Page: private std::vector<Line>,
		    public std::enable_shared_from_this<Page> {
	public:
		using Path = boost::filesystem::path;
		using Base = std::vector<Line>;
		using value_type = Base::value_type;
	
		Page(Book& book, int i, Box b = {})
			: book(book.shared_from_this())
			, box(b)
			, id(i) 
		{}

		using Base::begin;
		using Base::end;
		using Base::push_back;
		using Base::back;
		using Base::front;
		using Base::empty;
		using Base::size;
		
		const std::weak_ptr<Book> book;
		const Box box;
		const int id;
		Path ocr, img;
	};
}

#endif // pcw_Page_hpp__
