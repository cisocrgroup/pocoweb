#include <boost/algorithm/string.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/system/error_code.hpp>
#include <boost/log/trivial.hpp>
#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <regex>
#include <sstream>
#include "util.hpp"
#include "Config.hpp"
#include "BadRequest.hpp"
#include "Book.hpp"
#include "Page.hpp"
#include "Pix.hpp"
#include "BookDir.hpp"
#include "AltoXmlPageParser.hpp"
#include "LlocsPageParser.hpp"
#include "AbbyyXmlPageParser.hpp"
#include "HocrPageParser.hpp"

namespace fs = boost::filesystem;
using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
static BookDir::Path
create_unique_bookdir_path(const Config& config)
{
	BookDir::Path path(config.daemon.basedir);
	while (true) {
		auto id = gensessionid(16);
		auto dir = path / id;
		if (not fs::is_directory(dir)) {
			fs::create_directory(dir);
			return dir;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
BookDir::BookDir(const Config& config)
	: dir_(create_unique_bookdir_path(config))
{
	assert(fs::is_directory(dir_));
}

////////////////////////////////////////////////////////////////////////////////
BookDir::BookDir(Path path)
	: dir_(std::move(path))
{
	if (not fs::is_directory(dir_))
		throw std::logic_error("(BookDir) Not a directory: " + dir_.string());
}

////////////////////////////////////////////////////////////////////////////////
void 
BookDir::remove() const
{
	fs::remove_all(dir_);
}

////////////////////////////////////////////////////////////////////////////////
void 
BookDir::clean_up_tmp_dir() const
{
	fs::remove_all(tmp_dir());
}

////////////////////////////////////////////////////////////////////////////////
void 
BookDir::add_zip_file(const std::string& content)
{
	auto tdir = tmp_dir();
	fs::create_directory(tdir);
	auto zip = zip_file();
	std::ofstream os(zip.string());
	if (not os.good())
		throw std::system_error(errno, std::system_category(), zip.string());
	os << content;
	os.close();
	std::string command = "unzip -qq -d " + tdir.string() + " " + zip.string();
	// std::cerr << "COMMAND: " << command << "\n";
	auto err = system(command.data());
	if (err)
		throw std::runtime_error(command + " returned: " + std::to_string(err));
	fs::recursive_directory_iterator i(tdir), e;
	for (; i != e; ++i) {
		std::cerr << "CHECKING PATH: " << *i << "\n";
		add_file(*i);
	}
}

////////////////////////////////////////////////////////////////////////////////
void 
BookDir::add_file(const Path& path)
{
	auto type = get_file_type(path);

	switch (type) {
	case Type::Llocs:
		ocrs_[path.parent_path()] = type;
		break;
	case Type::Img:
		imgs_.push_back(path);
		break;
	case Type::AbbyyXml: // fall through
	case Type::Hocr: // fall through
	case Type::AltoXml:
		ocrs_[path] = type; 
		break;
	case Type::Other: // do nothing
		break;
	}
}

////////////////////////////////////////////////////////////////////////////////
BookPtr
BookDir::build() const
{
	const auto book = make_book();
	assert(book);
	fix(*book);
	setup(*book);
	return book;
}

////////////////////////////////////////////////////////////////////////////////
BookPtr
BookDir::make_book() const
{
	auto book = std::make_shared<Book>();
	for (const auto& ocr: ocrs_) {
		auto parser = get_page_parser(ocr);
		assert(parser);
		while (parser->has_next())
			book->push_back(parser->next());
	}
	return book;
}

////////////////////////////////////////////////////////////////////////////////
void
BookDir::fix(Book& book) const
{
	fix_page_ordering(book);
	fix_image_paths(book);
}

////////////////////////////////////////////////////////////////////////////////
void
BookDir::fix_indizes(Book& book)
{
	int id = 0;
	for (auto& page: book) {
		page->id = ++id;
	}
}
		
////////////////////////////////////////////////////////////////////////////////
void
BookDir::fix_image_paths(Book& book) const
{
	for (const auto& page: book) {
		fix_image_paths(*page);
	}
}

////////////////////////////////////////////////////////////////////////////////
void
BookDir::fix_image_paths(Page& page) const
{
	if (page.ocr.empty())
		return;

	const auto b = begin(imgs_);
	const auto e = end(imgs_);

	// find by the reference in the ocr source file
	auto i = std::find_if(b, e, [&page](const auto& path) {
		return path.filename() == page.img.filename();
	});
	if (i != e) {
		page.img = *i;
		return;
	}

	// find by matching file names
	i = std::find_if(b, e, [&page](const auto& path) {
		return path.stem() == page.ocr.stem();
	});	
	if (i != e) {
		page.img = *i;
		return;
	}

	// cannot find the image file
	throw BadRequest(
		"(BookDir) Cannot find image file for " +
		page.ocr.string() + " (" + page.img.string() + ")"
	);
}

////////////////////////////////////////////////////////////////////////////////
void
BookDir::fix_page_ordering(Book& book) 
{
	const auto b = begin(book);
	const auto e = end(book);

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
		fix_indizes(book);
	} else {
		// sort by path stem
		std::sort(b, e, [](const auto& a, const auto& b) {
			return a->ocr.stem() < b->ocr.stem();
		});
		fix_indizes(book);
	}
}

////////////////////////////////////////////////////////////////////////////////
PageParserPtr
BookDir::get_page_parser(const Ocrs::value_type& ocr) 
{
	switch (ocr.second) {
	case Type::Hocr:
		return std::make_unique<HocrPageParser>(ocr.first);
	case Type::AltoXml:
		return std::make_unique<AltoXmlPageParser>(ocr.first);
	case Type::AbbyyXml:
		return std::make_unique<AbbyyXmlPageParser>(ocr.first);
	case Type::Llocs:
		return std::make_unique<LlocsPageParser>(ocr.first);
	default:
		throw std::logic_error("(BookDir) Invalid file type");
	}
}

////////////////////////////////////////////////////////////////////////////////
BookDir::Type 
BookDir::get_file_type(const Path& path)
{
	static const std::regex html{R"(\.html?$)", std::regex_constants::icase};
	static const std::regex xml{R"(\.xml$)", std::regex_constants::icase};
	static const std::regex llocs{R"(\.llocs$)"};
	static const std::regex img{
		R"(\.((png)|(jpe?g)|(tiff?))$)",
		std::regex_constants::icase
	};
	auto str = path.string();
	if (std::regex_search(str, img))
		return Type::Img;
	if (std::regex_search(str, llocs))
		return Type::Llocs;
	if (std::regex_search(str, xml)) 
		return get_xml_file_type(path);
	if (std::regex_search(str, html))
		return Type::Hocr;
	return Type::Other;
}

////////////////////////////////////////////////////////////////////////////////
BookDir::Type 
BookDir::get_xml_file_type(const Path& path)
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
		return Type::AbbyyXml;
	if (std::search(buf, buf + n, begin(alto), end(alto)) != buf + n)
		return Type::AltoXml;
	if (std::search(buf, buf + n, begin(hocr), end(hocr)) != buf + n)
		return Type::Hocr;
	return Type::Other;
}
	
////////////////////////////////////////////////////////////////////////////////
void
BookDir::setup(const Book& book) const
{
	for (const auto& page: book) {
		assert(page);
		auto pagedir = make_page_directory(*page);
		copy_img_and_ocr_files(pagedir, *page);
		make_line_img_files(pagedir, *page);
	}
}

////////////////////////////////////////////////////////////////////////////////
BookDir::Path
BookDir::make_page_directory(const Page& page) const
{
	Path pagedir = dir_ / path_from_id(page.id);
	fs::create_directory(pagedir);
	return pagedir;
}

////////////////////////////////////////////////////////////////////////////////
void
BookDir::copy_img_and_ocr_files(const Path& pagedir, Page& page) const
{
	if (page.ocr.empty())
		return;
	auto tocr = pagedir / page.ocr.filename();
	copy(page.ocr, tocr);
	auto timg = pagedir/ page.img.filename();
	copy(page.img, timg);
	fs::create_hard_link(page.img, timg);
	page.ocr = tocr;
	page.img = timg;
}

////////////////////////////////////////////////////////////////////////////////
void
BookDir::make_line_img_files(const Path& pagedir, Page& page) const
{
	PixPtr pix; 
	if (not page.img.empty()) {
		pix.reset(pixRead(page.img.string().data()));
		if (not pix)
			throw std::runtime_error("(BookDir) Cannot read image " + page.img.string());
	}
	for (auto& line: page) {
		if (line.img.empty() and not pix) {
			throw std::runtime_error(
				"(BookDir) Missing line image for line: " + 
				line.string()
			);
		} else if (line.img.empty() and pix) {
			line.img = pagedir / path_from_id(line.id);
			line.img.replace_extension(page.img.extension());
			write_line_img_file(pix.get(), line);
		} else {
			auto tfile = pagedir / line.img.filename();
			copy(line.img, tfile);
			line.img = tfile;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
void
BookDir::write_line_img_file(void *vpix, const Line& line)
{
	auto pix = (PIX*)vpix;
	assert(pix);
	auto format = pixGetInputFormat(pix);
	BOX box {
		.x = line.box.left(), 
		.y = line.box.top(), 
		.w = line.box.width(), 
		.h = line.box.height()
	};

	if (box.x + box.w <= (int) pix->w and box.y + box.h <= (int) pix->h) {
		PixPtr tmp{pixClipRectangle(pix, &box, nullptr)};
		if (not tmp or pixWrite(line.img.string().data(), tmp.get(), format))
			throw std::runtime_error("(BookDir) Cannot write img " + line.img.string());
	} else {
		std::cerr << "[WARNING] cannot write line image for " << line.string() << "\n";
	}
}

////////////////////////////////////////////////////////////////////////////////
void
BookDir::copy(const Path& from, const Path& to)
{
	// fs::create_hard_link(to, from); TODO: ??!!
	boost::system::error_code ec;
	fs::create_hard_link(from, to);
	if (ec) { // could not create hard link; try copy
		fs::copy_file(from, to);
	}
}

////////////////////////////////////////////////////////////////////////////////
BookDir::Path
BookDir::path_from_id(int id)
{
	std::stringstream os;
	os << std::hex << std::setw(10) << std::setfill('0') << id;
	return os.str();
}
