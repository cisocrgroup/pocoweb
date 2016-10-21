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
		void setup_directory_structure(Book& book) const;
		Path make_page_directory(const Page& page) const;
		void copy_img_and_ocr_files(const Path& pagedir, Page& page) const;
		void make_line_img_files(const Path& pagedir, Page& page) const;

		static uint32_t get_img_format(const Path& path) noexcept;
		static void write_line_img_file(void* pix, const Line& line, uint32_t format);
		static bool is_ocr_file(const Path& dir);
		static bool is_img_file(const Path& dir);
		static Path path_from_id(int id);

		const Path path_;
	};
}

#endif // pcw_BookDir_hpp__

