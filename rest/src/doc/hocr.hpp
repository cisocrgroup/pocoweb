#ifndef pcw_hocr_hpp__
#define pcw_hocr_hpp__

namespace pcw {
	class Book;
	using BookPtr = std::shared_ptr<Book>;
	BookPtr parse_hocr(const std::string& dir);
}

#endif // pcw_hocr_hpp__
