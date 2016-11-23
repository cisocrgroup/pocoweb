#include <regex>
#include <fstream>
#include "parser/ParserPage.hpp"
#include "util.hpp"
#include "Page.hpp"
#include "Error.hpp"
#include "BookBuilder.hpp"
#include "Book.hpp"
#include "MetsXmlBookParser.hpp"
#include "parser/PageParser.hpp"

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
BookPtr
BookBuilder::build() const
{
	auto i = std::find_if(begin(ocr_), end(ocr_), [](const auto& p) {
		return p.second == FileType::Mets;
	});
	if (i == end(ocr_)) {
		return build(parse_book_data());
	} else {
		MetsXmlBookParser p(i->first);
		return p.parse();
	}
}

////////////////////////////////////////////////////////////////////////////////
void
BookBuilder::add(const Path& file)
{
	auto type = get_file_type(file);
	switch (type) {
	case FileType::Img:
		img_.push_back(file);
		break;
	case FileType::Llocs:
		ocr_[file.parent_path()] = type;
		break;
	case FileType::Other:
		// do nothing
		break;
	default:
		ocr_[file] = type;
		break;
	}
}

////////////////////////////////////////////////////////////////////////////////
BookBuilder::BookData
BookBuilder::parse_book_data() const
{
	BookData data;
	for (const auto& ocr: ocr_) {
		std::cerr << "OCR: " << ocr.first << "\n";
		auto pp = make_page_parser(ocr.second, ocr.first);
		assert(pp);
		while (pp->has_next()) {
			auto page = pp->parse()->page();
			if (page->has_ocr_path() and not page->has_img_path()) {
				auto i = find_matching_img_file(page->ocr);
				if (i == end(img_))
					THROW(BadRequest, "(BookBuilder) Unable to find ",
							"matching img file for ", page->ocr);
			}
			data.pages.push_back(page);
		}
	}
	order_pages(data.pages);
	return data;
}

////////////////////////////////////////////////////////////////////////////////
BookPtr
BookBuilder::build(const BookData& data)
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
BookBuilder::ImgFiles::const_iterator
BookBuilder::find_matching_img_file(const Path& ocr) const noexcept
{
	auto stem = ocr.stem();
	return std::find_if(begin(img_), end(img_), [&stem](const Path& path) {
		return path.stem() == stem;
	});
}

////////////////////////////////////////////////////////////////////////////////
void
BookBuilder::order_pages(std::vector<PagePtr>& pages) noexcept
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
BookBuilder::fix_indizes(std::vector<PagePtr>& pages) noexcept
{
	int id = 0;
	for (auto& page: pages) {
		page->id = ++id;
	}
}
