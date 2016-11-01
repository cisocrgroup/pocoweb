#ifndef pcw_BookBuilder_hpp__
#define pcw_BookBuilder_hpp__

#include <boost/filesystem/path.hpp>
#include <memory>
#include <vector>

namespace pcw {
	class Book;
	class Page;
	class PageParser;
	using BookPtr = std::shared_ptr<Book>;
	using PagePtr = std::shared_ptr<Page>;
	using PageParserPtr = std::unique_ptr<PageParser>;
	using Path = boost::filesystem::path;

	class BookBuilder {
	public:
		
		BookBuilder() = default;
		virtual ~BookBuilder() noexcept = default;
		void add(const Path& file);
		BookPtr build() const;
		
	private:
		enum class FileType {
			Other, Img, AltoXml, AbbyyXml, Hocr, Llocs
		};
		using FilePair = std::pair<Path, FileType>;
		using OcrFiles = std::vector<FilePair>;
		using ImgFiles = std::vector<Path>;
		struct BookData {
			std::vector<PagePtr> pages;
			std::string author, title, description, uri;
			Path dir;
			int year;
		};
		virtual BookData parse_book_data() const;
		static BookPtr build(const BookData& data);
		ImgFiles::const_iterator find_matching_img_file(const Path& ocr) const noexcept;
		static PageParserPtr get_page_parser(const FilePair& type);
		static FileType get_file_type(const Path& path);
		static FileType get_xml_file_type(const Path& path);
		static void order_pages(std::vector<PagePtr>& pages) noexcept;
		static void fix_indizes(std::vector<PagePtr>& pages) noexcept;

		OcrFiles ocr_;
		ImgFiles img_;
	};
}

#endif // pcw_BookBuilder_hpp__
