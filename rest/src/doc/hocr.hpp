#ifndef pcw_hocr_hpp__
#define pcw_hocr_hpp__

#include <boost/filesystem/path.hpp>

namespace pcw {
	using Path = boost::filesystem::path;
	class Book;
	using BookPtr = std::shared_ptr<Book>;
	class Page;
	using PagePtr = std::shared_ptr<Page>;

	BookPtr parse_hocr_book(const Path& dir);
	PagePtr parse_hocr_page(const Path& file);
}

#endif // pcw_hocr_hpp__
