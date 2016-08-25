#ifndef pcw_BookDir_hpp__
#define pcw_BookDir_hpp__

#include <memory>
#include <boost/filesystem/path.hpp>

namespace pcw {
	class Book;
	class Page;
	using PagePtr = std::shared_ptr<Page>;

	class BookDir {
	public:
		BookDir(const Book& book);

		// first create the image...
		PagePtr add_page_image(int id, const std::string& ext, std::istream& is) const;
		// ... then add the ocr (xml) file
		void add_page_ocr(Page& page, std::istream& is) const;
		void add_line_images(Page& page) const;

	private:
		using Path = boost::filesystem::path;
		void add_line_image(Line& line, const Path& dir, void *pix) const;
		Path get_page(int id) const;
		static void copy(std::istream& is, const Path& o);
		static std::string get_hex_str(int id);

		const Path path_;
	};
}

#endif // pcw_BookDir_hpp__

