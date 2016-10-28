#ifndef pcw_Page_hpp__
#define pcw_Page_hpp__

#include <boost/filesystem/path.hpp>
#include <memory>
#include <vector>
#include "Box.hpp"
#include "Book.hpp"
#include "Line.hpp"

namespace pcw {
	class Project;
	class Book;
	using ConstBookPtr = std::shared_ptr<const Book>;
	class Page;
	using PagePtr = std::shared_ptr<Page>;

	class Page: private std::vector<Line>,
		    public std::enable_shared_from_this<Page> {
	public:
		using Path = boost::filesystem::path;
		using Base = std::vector<Line>;
		using value_type = Base::value_type;

		Page(int i, Box b = {})
			: box(b)
			, id(i) 
			, ocr()
			, img()
			, book_()
		{}
	
		using Base::begin;
		using Base::end;
		using Base::back;
		using Base::front;
		using Base::empty;
		using Base::size;
		ConstBookPtr book() const noexcept {return book_.lock();}
			
		void push_back(const Line& line) {
			Base::push_back(line);
			this->back().page_ = shared_from_this();
		}
		void push_back(Line&& line) {
			Base::push_back(std::forward<Line>(line));
			this->back().page_ = shared_from_this();
		}
		bool has_ocr_path() const noexcept {return not ocr.empty();}
		bool has_img_path() const noexcept {return not img.empty();}
		
		Box box;
		int id;
		Path ocr, img;

	private:
		std::weak_ptr<const Book> book_;
		friend class Project;
	};
}

#endif // pcw_Page_hpp__
