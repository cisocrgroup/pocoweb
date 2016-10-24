#ifndef pcw_BookDir_hpp__
#define pcw_BookDir_hpp__

#include <memory>
#include <boost/filesystem/path.hpp>
#include <vector>
#include <map>
#include <stdexcept>

namespace pcw {
	class Book;
	using BookPtr = std::shared_ptr<Book>;
	class Page;
	class Line;
	struct Config;
	using PagePtr = std::shared_ptr<Page>;
	class PageParser;
	using PageParserPtr = std::unique_ptr<PageParser>;
	
	class BookDir {
	public:
		using Path = boost::filesystem::path;

		// create a new Bookdir
		BookDir(const Config& config);
		// opens an existing Bookdir
		BookDir(Path path);

		const Path& dir() const noexcept {return dir_;}
		Path tmp_dir() const noexcept {return dir_ / ".tmp";}
		Path zip_file() const noexcept {return tmp_dir() / "book.zip";}
		Path img_dir() const noexcept {return dir_ / "img";}
		Path ocr_dir() const noexcept {return dir_ / "ocr";}

		void remove() const;
		void clean_up_tmp_dir() const;
		void add_zip_file(const std::string& content);
		void add_file(const Path& path);
		BookPtr build() const;

	private:
		enum class Type {
			Other, Img, AltoXml, AbbyyXml, Hocr, Llocs
		};
		using Ocrs = std::map<Path, Type>;
		using Imgs = std::vector<Path>;

		BookPtr make_book() const;
		void fix(Book& book) const;
		void setup(const Book& book) const;
		void fix_image_paths(Book& book) const;
		void fix_image_paths(Page& page) const;

		void setup(const Path& dir, Book& book) const;
		void setup_directory_structure(Book& book) const;
		Path make_page_directory(const Page& page) const;
		void copy_img_and_ocr_files(const Path& pagedir, Page& page) const;
		void make_line_img_files(const Path& pagedir, Page& page) const;

		static void copy(const Path& from, const Path& to);
		static void fix_page_ordering(Book& book);
		static void fix_indizes(Book& book);
		static Type get_file_type(const Path& path);
		static Type get_xml_file_type(const Path& path);
		static PageParserPtr get_page_parser(const Ocrs::value_type& v);
		static void write_line_img_file(void* pix, const Line& line);
		static Path path_from_id(int id);

		const Path dir_;
		std::map<Path, Type> ocrs_;
		std::vector<Path> imgs_;
	};
}

#endif // pcw_BookDir_hpp__

