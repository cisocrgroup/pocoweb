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

		Page(int i, Box b = {})
			: book()
			, box(b)
			, id(i) 
			, ocr()
			, img()
		{}
	
		using Base::begin;
		using Base::end;
		using Base::back;
		using Base::front;
		using Base::empty;
		using Base::size;
		void push_back(const Line& line) {
			this->push_back(line);
			this->back().page = shared_from_this();
		}
		void push_back(Line&& line) {
			this->push_back(line);
			this->back().page = shared_from_this();
		}
		
		
		std::weak_ptr<Book> book;
		const Box box;
		int id;
		Path ocr, img;
	};
}

#endif // pcw_Page_hpp__
