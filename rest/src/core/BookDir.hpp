#ifndef pcw_BookDir_hpp__
#define pcw_BookDir_hpp__

#include <memory>
#include <boost/filesystem/path.hpp>
#include <stdexcept>

namespace pcw {
	class Book;
	class Page;
	class Line;
	struct Config;
	using PagePtr = std::shared_ptr<Page>;
	
	class BookDir {
	public:
		using Path = boost::filesystem::path;

		// create a new Bookdir
		BookDir(const Config& config);
		// opens an existing Bookdir
		BookDir(const std::string& path);

		// first create the image...
		PagePtr add_page_image(int id, const std::string& ext, std::istream& is) const;
		// ... then add the ocr (xml) file
		void add_page_ocr(Page& page, std::istream& is) const;
		void add_line_images(Page& page) const;
		const Path& path() const noexcept {return path_;}
		Path tmp_dir() const noexcept {return path_ / ".tmp";}
		Path zip_file() const noexcept {return tmp_dir() / "book.zip";}
		Path img_dir() const noexcept {return path_ / "img";}
		Path ocr_dir() const noexcept {return path_ / "ocr";}

		void remove() const;
		void clean_up() const;
		void setup(const std::string& str, Book& book) const;

	private:
		using Paths = std::vector<Path>;

		void setup(const Path& dir, Book& book) const;
		void add_pages(const Path& path, Book& book) const;
		void add_image_path(const Paths& img, Page& page) const;
		static bool is_ocr_file(const Path& dir);
		static bool is_img_file(const Path& dir);

		void add_line_image(Line& line, const Path& dir, void *pix) const;
		Path get_page(int id) const;
		static void copy(std::istream& is, const Path& o);
		static std::string get_hex_str(int id);

		const Path path_;
	};
}

#endif // pcw_BookDir_hpp__

