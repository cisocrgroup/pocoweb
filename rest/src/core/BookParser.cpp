#include <regex>
#include <fstream>
#include "Page.hpp"
#include "BadRequest.hpp"
#include "BookParser.hpp"
#include "Book.hpp"
#include "AltoXmlPageParser.hpp"
#include "LlocsPageParser.hpp"
#include "AbbyyXmlPageParser.hpp"
#include "HocrPageParser.hpp"

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
BookPtr
BookParser::build() const
{
	return build(parse_book_data());
}

////////////////////////////////////////////////////////////////////////////////
void 
BookParser::add(const Path& file)
{
	auto type = get_file_type(file);
	switch (type) {
	case FileType::Img:
		img_.push_back(file);
		break;
	case FileType::Other:
		// do nothing
		break;
	default:
		ocr_.emplace_back(file, type);
		break;
	}
}

////////////////////////////////////////////////////////////////////////////////
BookParser::BookData 
BookParser::parse_book_data() const
{
	BookData data;
	for (const auto& ocr: ocr_) {
		auto pp = get_page_parser(ocr);
		assert(pp);
		while (pp->has_next()) {
			auto page = pp->parse();
			if (not page->has_img_path()) {
				auto i = find_matching_img_file(page->ocr);
				if (i == end(img_))
					throw BadRequest(
						"(BookParser) Unable to find "
						"matching image file for: " + 
						page->ocr.string()
					);
			}	
			data.pages.push_back(page);
		}
	}
	order_pages(data.pages);
	return data;
}

////////////////////////////////////////////////////////////////////////////////
BookPtr 
BookParser::build(const BookData& data)
{
	auto book = std::make_shared<Book>();
	book->description = data.description;
	book->uri = data.uri;
	book->author = data.author;
	book->title = data.title;
	book->description = data.description;
	for (const auto& page: data.pages)
		book->push_back(page);
	return book;
}

////////////////////////////////////////////////////////////////////////////////
BookParser::ImgFiles::const_iterator 
BookParser::find_matching_img_file(const Path& ocr) const noexcept
{
	auto stem = ocr.stem();
	return std::find_if(begin(img_), end(img_), [&stem](const Path& path) {
		return path.stem() == stem;
	});
}

////////////////////////////////////////////////////////////////////////////////
PageParserPtr 
BookParser::get_page_parser(const FilePair& ocr)
{
	switch (ocr.second) {
	case FileType::Hocr:
		return std::make_unique<HocrPageParser>(ocr.first);
	case FileType::AltoXml:
		return std::make_unique<AltoXmlPageParser>(ocr.first);
	case FileType::AbbyyXml:
		return std::make_unique<AbbyyXmlPageParser>(ocr.first);
	case FileType::Llocs:
		return std::make_unique<LlocsPageParser>(ocr.first);
	default:
		throw std::logic_error("(BookBuilder) Invalid file type");
	}
}

////////////////////////////////////////////////////////////////////////////////
BookParser::FileType
BookParser::get_xml_file_type(const Path& path) 
{
	static const std::string abbyy{"http://www.abbyy.com"};
	static const std::string alto{"<alto"};
	static const std::string hocr{"<html"};

	std::ifstream is(path.string());
	if (not is.good())
		throw std::system_error(errno, std::system_category(), path.string());
	char buf[1024];
	is.read(buf, 1024);
	const auto n = is.gcount();
	if (std::search(buf, buf + n, begin(abbyy), end(abbyy)) != buf + n)
		return FileType::AbbyyXml;
	if (std::search(buf, buf + n, begin(alto), end(alto)) != buf + n)
		return FileType::AltoXml;
	if (std::search(buf, buf + n, begin(hocr), end(hocr)) != buf + n)
		return FileType::Hocr;
	return FileType::Other;
}

////////////////////////////////////////////////////////////////////////////////
BookParser::FileType 
BookParser::get_file_type(const Path& path)
{
	static const std::regex hocr{
		R"(\.((html?)|(hocr))$)", 
		std::regex_constants::icase
	};
	static const std::regex xml{R"(\.xml$)", std::regex_constants::icase};
	static const std::regex llocs{R"(\.llocs$)"};
	static const std::regex img{
		R"(\.((png)|(jpe?g)|(tiff?))$)",
		std::regex_constants::icase
	};
	auto str = path.string();
	if (std::regex_search(str, img))
		return FileType::Img;
	if (std::regex_search(str, llocs))
		return FileType::Llocs;
	if (std::regex_search(str, xml)) 
		return get_xml_file_type(path);
	if (std::regex_search(str, hocr))
		return FileType::Hocr;
	return FileType::Other;
}

////////////////////////////////////////////////////////////////////////////////
void 
BookParser::order_pages(std::vector<PagePtr>& pages) noexcept
{
	const auto b = begin(pages);
	const auto e = end(pages);

	if (std::all_of(b, e, [](const auto& page) {return page->id > 0;})) {
		// sort by page index of ocr source file
		std::sort(b, e, [](const auto& a, const auto& b) {
			return a->id < b->id;
		});
	} else if (std::any_of(b, e, [](const auto& page) {return page->id > 0;})) {
		// sort by page index AND path stem
		std::sort(b, e, [](const auto& a, const auto& b) {
			if (a->id > 0 and b->id > 0) {
				return a->id < b->id;
			} else {
				return a->ocr.stem() < b->ocr.stem();
			}
		});
		fix_indizes(pages);
	} else {
		// sort by path stem
		std::sort(b, e, [](const auto& a, const auto& b) {
			return a->ocr.stem() < b->ocr.stem();
		});
		fix_indizes(pages);
	}
}

////////////////////////////////////////////////////////////////////////////////
void 
BookParser::fix_indizes(std::vector<PagePtr>& pages) noexcept
{
	int id = 0;
	for (auto& page: pages) {
		page->id = ++id;
	}
}
