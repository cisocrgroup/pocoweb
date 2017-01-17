#ifndef pcw_BookConstructor_hpp__
#define pcw_BookConstructor_hpp__

#include <boost/filesystem/path.hpp>
#include <map>
#include <memory>
#include <vector>
#include "util.hpp"

namespace pcw {
	class Book;
	class Page;
	class PageParser;
	using BookPtr = std::shared_ptr<Book>;
	using PagePtr = std::shared_ptr<Page>;
	using PageParserPtr = std::unique_ptr<PageParser>;
	using Path = boost::filesystem::path;

	class BookConstructor {
	public:
		void add(const Path& file);
		BookPtr build() const;

	private:
		using OcrFiles = std::map<Path, FileType>;
		using FilePair = OcrFiles::value_type;
		using ImgFiles = std::vector<Path>;
		struct BookData {
			std::vector<PagePtr> pages;
			std::string author, title, description, uri;
			Path dir;
			int year;
		};
		BookData parse_book_data() const;
		static BookPtr build(const BookData& data);
		ImgFiles::const_iterator find_matching_img_file(const Path& ocr) const noexcept;
		static void order_pages(std::vector<PagePtr>& pages) noexcept;
		static void fix_indizes(std::vector<PagePtr>& pages) noexcept;

		OcrFiles ocr_;
		ImgFiles img_;
	};
}

#endif // pcw_BookConstructor_hpp__
