#ifndef pcw_BookDirectoryBuilder_hpp__
#define pcw_BookDirectoryBuilder_hpp__

#include <boost/filesystem/path.hpp>
#include <map>
#include <memory>
#include <stdexcept>
#include <vector>
#include "BookConstructor.hpp"

namespace pcw {
class Book;
using BookPtr = std::shared_ptr<Book>;
class Page;
class Line;
struct Config;
using PagePtr = std::shared_ptr<Page>;
class PageParser;
using PageParserPtr = std::unique_ptr<PageParser>;

class BookDirectoryBuilder {
       public:
	using Path = boost::filesystem::path;

	// create a new Bookdir
	BookDirectoryBuilder(const Config& config);
	// opens an existing Bookdir
	BookDirectoryBuilder(Path base, Path dir);
	~BookDirectoryBuilder() noexcept;

	Path dir() const noexcept { return base_dir_ / dir_; }
	Path tmp_dir() const noexcept { return base_dir_ / dir_ / tmp_dir_; }
	Path line_img_dir() const noexcept {
		return base_dir_ / dir_ / line_img_dir_;
	}
	Path img_dir() const noexcept { return base_dir_ / dir_ / "img"; }
	Path ocr_dir() const noexcept { return base_dir_ / dir_ / "ocr"; }

	void remove() const;
	void add_zip_file_content(const std::string& content);
	void add_zip_file_path(const std::string& content);
	void add_file(const Path& path);
	BookPtr build() const;

       private:
	Path zip_file() const noexcept { return tmp_dir() / "book.zip"; }
	// BookPtr make_book() const;
	void setup(const Book& book) const;
	void setup(const Path& dir, Book& book) const;
	void setup_directory_structure(Book& book) const;
	void setup_img_and_ocr_files(Page& page) const;
	void make_line_img_files(const Path& pagedir, Page& page) const;
	void unzip();
	void write_line_img_file(void* pix, const Line& line) const;

	static void copy(const Path& from, const Path& to);
	static Path path_from_id(int id);
	static Path remove_common_base_path(const Path& p, const Path& base);

	const Path base_dir_, dir_, tmp_dir_, line_img_dir_;
	BookConstructor builder_;
};
}

#endif  // pcw_BookDirectoryBuilder_hpp__
