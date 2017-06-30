#include <regex>
#include <fstream>
#include <crow/logging.h>
#include "parser/ParserPage.hpp"
#include "util.hpp"
#include "Page.hpp"
#include "utils/Error.hpp"
#include "BookConstructor.hpp"
#include "BookBuilder.hpp"
#include "Book.hpp"
#include "MetsXmlBookParser.hpp"
#include "parser/PageParser.hpp"

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
BookPtr
BookConstructor::build() const
{
	auto i = std::find_if(begin(ocr_), end(ocr_), [](const auto& p) {
		return p.second == FileType::Mets;
	});
	if (i == end(ocr_)) {
		return build(parse_book_info());
	} else {
		MetsXmlBookParser p(i->first);
		return p.parse();
	}
}

////////////////////////////////////////////////////////////////////////////////
void
BookConstructor::add(const Path& file)
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
BookConstructor::BookInfo
BookConstructor::parse_book_info() const
{
	BookInfo info;
	for (const auto& ocr: ocr_) {
		CROW_LOG_DEBUG << "parsing " << ocr.first
			       << ": " << file_type_to_string(ocr.second);
		// std::cerr << "OCR: " << ocr.first << "\n";
		auto pp = make_page_parser(ocr.second, ocr.first);
		assert(pp);
		while (pp->has_next()) {
			auto page = pp->parse()->page();
			assert(page);
			set_img_file(*page);
			if (page->has_ocr_path() and not page->has_img_path()) {
				auto i = find_matching_img_file(page->ocr);
				if (i == end(img_))
					THROW(BadRequest, "(BookConstructor) Unable to find ",
							"matching img file for ", page->ocr);
			}
			info.pages.push_back(page);
		}
	}
	order_pages(info.pages);
	return info;
}

////////////////////////////////////////////////////////////////////////////////
void
BookConstructor::set_img_file(Page& page) const
{
	if (not page.has_ocr_path()) // skip pages with no ocr file path
		return;
	// Some ocr files contain garbage image file paths.
	// Search for the best matching file in the img_ array.
	if (page.has_img_path()) {
		const auto img = find_matching_img_file(page.img);
		if (img == end(img_)) {
			// try again
			page.img = Path();
		} else {
			page.img = *img;
		}
	}

	// If no image could be found, search by ocr path
	if (not page.has_img_path()) {
		const auto img = find_matching_img_file(page.ocr);
		if (img != end(img_))
			page.img = *img;
	}
	if (not page.has_img_path()) {
		THROW(BadRequest, "(BookConstruct) Unable to find ",
				"matching image file for ", page.ocr);
	}
	CROW_LOG_INFO << "(BookConstructor) Found page: "
		      << page.ocr << " with image file: " << page.img;
}

////////////////////////////////////////////////////////////////////////////////
BookPtr
BookConstructor::build(const BookInfo& info)
{
	BookData data;
	data.description = info.description;
	data.uri = info.uri;
	data.author = info.author;
	data.title = info.title;
	data.lang = info.lang;
	data.description = info.description;
	BookBuilder builder;
	builder.set_book_data(data);
	for (auto& page: info.pages)
		builder.append(*page);
	return builder.build();
}

////////////////////////////////////////////////////////////////////////////////
BookConstructor::ImgFiles::const_iterator
BookConstructor::find_matching_img_file(const Path& path) const noexcept
{
	CROW_LOG_DEBUG << "(BookConstructor) Searching by full path: " << path;
	auto f = std::find_if(begin(img_), end(img_), [&](const auto& ipath) {
		return ipath == path;
	});
	if (f != end(img_))
		return f;

	const auto fn = path.filename();
	CROW_LOG_DEBUG << "(BookConstructor) Searching by file name: " << fn;
	f = std::find_if(begin(img_), end(img_), [&](const auto& ipath) {
		return ipath.filename() == fn;
	});
	if (f != end(img_))
		return f;

	const auto stem = path.filename().stem();
	CROW_LOG_DEBUG << "(BookConstructor) Searching by file stem: " << stem;
	f = std::find_if(begin(img_), end(img_), [&](const auto& ipath) {
		return ipath.filename().stem() == stem;
	});
	if (f == end(img_))
		CROW_LOG_WARNING << "(BookConstructor) Could not find image file for path: " << path;
	return f;
}

////////////////////////////////////////////////////////////////////////////////
void
BookConstructor::order_pages(std::vector<PagePtr>& pages) noexcept
{
	const auto b = begin(pages);
	const auto e = end(pages);

	if (std::all_of(b, e, [](const auto& page) {return page->id() > 0;})) {
		// sort by page index of ocr source file
		std::sort(b, e, [](const auto& a, const auto& b) {
			return a->id() < b->id();
		});
	} else if (std::any_of(b, e, [](const auto& page) {return page->id() > 0;})) {
		// sort by page index AND path stem
		std::sort(b, e, [](const auto& a, const auto& b) {
			if (a->id() > 0 and b->id() > 0) {
				return a->id() < b->id();
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
BookConstructor::fix_indizes(std::vector<PagePtr>& pages) noexcept
{
	int id = 0;
	for (auto& page: pages) {
		page->set_id(++id);
	}
}
