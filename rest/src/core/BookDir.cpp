#include <boost/filesystem/operations.hpp>
#include <boost/log/trivial.hpp>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include "util.hpp"
#include "Config.hpp"
#include "BadRequest.hpp"
#include "Book.hpp"
#include "Page.hpp"
#include "Pix.hpp"
#include "BookDir.hpp"
#include "XmlPageParsing.hpp"

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
	: path_(create_unique_bookdir_path(config))
{
	assert(fs::is_directory(path_));
}

////////////////////////////////////////////////////////////////////////////////
BookDir::BookDir(const std::string& path)
	: path_(path)
{
	if (not fs::is_directory(path_))
		throw std::logic_error("(BookDir) Not a directory: " + path_.string());
}

////////////////////////////////////////////////////////////////////////////////
void 
BookDir::remove() const
{
	fs::remove_all(path_);
}

////////////////////////////////////////////////////////////////////////////////
void 
BookDir::clean_up() const
{
	fs::remove_all(tmp_dir());
}

////////////////////////////////////////////////////////////////////////////////
void 
BookDir::setup(const std::string& str, Book& book) const
{
	auto tdir = tmp_dir();
	fs::create_directory(tdir);
	auto zip = zip_file();
	std::ofstream os(zip.string());
	if (not os.good())
		throw std::system_error(errno, std::system_category(), zip.string());
	os << str;
	os.close();
	std::string command = "unzip -qq -d " + tdir.string() + " " + zip.string();
	// std::cerr << "COMMAND: " << command << "\n";
	auto err = system(command.data());
	if (err)
		throw std::runtime_error(command + " returned: " + std::to_string(err));
	setup(tdir, book);
}

////////////////////////////////////////////////////////////////////////////////
void 
BookDir::setup(const Path& dir, Book& book) const
{
	assert(fs::is_directory(dir));
	fs::recursive_directory_iterator i(dir), e;
	Paths imgs;
	for (; i != e; ++i) {
		std::cerr << "CHECKING PATH: " << *i << "\n";
		if (is_ocr_file(*i)) {
			add_pages(*i, book);
		} else if (is_img_file(*i)) {
			imgs.push_back(*i);
		}
	}
	for (const auto& page: book) {
		if (page) {
			add_image_path(imgs, *page);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
void
BookDir::add_pages(const Path& ocr, Book& book) const
{
	std::cerr << "OCR FILE: " << ocr << "\n";
	pcw::add_pages(ocr, book);
}

////////////////////////////////////////////////////////////////////////////////
void
BookDir::add_image_path(const Paths& imgs, Page& page) const
{
	auto i = std::find_if(begin(imgs), end(imgs), [&page](const auto& path) {
		return path.filename() == page.img.filename();
	});
	if (i == end(imgs)) 
		throw BadRequest("(BookDir) Missing image: " + page.img.string());
	page.img = *i;
}

////////////////////////////////////////////////////////////////////////////////
bool
BookDir::is_ocr_file(const Path& path)
{
	return fs::is_regular_file(path) and path.extension() == ".xml";
}

////////////////////////////////////////////////////////////////////////////////
bool
BookDir::is_img_file(const Path& path)
{
	return fs::is_regular_file(path) and (
		path.extension() == ".jpg" or 
		path.extension() == ".jpeg" or
		path.extension() == ".tif" or
		path.extension() == ".tiff" or
		path.extension() == ".png"
	);
}

////////////////////////////////////////////////////////////////////////////////
void 
BookDir::add_page_ocr(Page& page, std::istream& is) const 
{
	// page.ocrfile = get_page(page.id);
	// page.ocrfile.replace_extension("xml");
	// BOOST_LOG_TRIVIAL(debug) << "(BookDir::add_page_ocr) ofile: " << page.ocrfile;
	// copy(is, page.ocrfile);
	// page.parse();
	// add_line_images(page);
}

////////////////////////////////////////////////////////////////////////////////
PagePtr 
BookDir::add_page_image(int id, const std::string& ext, std::istream& is) const 
{
	// auto page = std::make_shared<Page>(id);
	// page->imagefile = get_page(id);
	// page->imagefile.replace_extension(ext);
	// BOOST_LOG_TRIVIAL(debug) << "(BookDir::add_page_image) ofile: " << page->imagefile;
	// copy(is, page->imagefile);
	// return page;
	return nullptr;
}

// #define VAR(x) #x << "=" << x

////////////////////////////////////////////////////////////////////////////////
void 
BookDir::add_line_images(Page& page) const
{
	// auto dir = get_page(page.id);
	// fs::create_directory(dir);
	// PixPtr pix{pixRead(page.imagefile.string().data())};
	// 
	// for (auto& line: page) {
	// 	if (line)
	// 		add_line_image(*line, dir, pix.get());
	// }
}

////////////////////////////////////////////////////////////////////////////////
void 
BookDir::add_line_image(Line& line, const Path& dir, void *ppix) const
{
	// // TODO: remove void*
	// auto file = dir / ("line-" + get_hex_str(line.id));
	// file.replace_extension("png");	
	// line.imagefile = file.string();
	// 
	// auto pix = static_cast<PIX*>(ppix);
	// if (pix) {
	// 	// only works iff the same order of the struct's definition is used
	// 	BOX box {
	// 		.x = line.box.left,
	// 		.y = line.box.top,
	// 		.w = line.box.width(),
	// 		.h = line.box.height(),
	// 		.refcount = 0 
	// 	};
	// 	PixPtr tmp{pixClipRectangle(pix, &box, nullptr)};
	// 	if (tmp) 
	// 		pixWrite(line.imagefile.string().data(), tmp.get(), IFF_PNG);
	// }
}

////////////////////////////////////////////////////////////////////////////////
BookDir::Path
BookDir::get_page(int id) const
{
	//return path_ / ("page-" + get_hex_str(id));
	return {};
}

////////////////////////////////////////////////////////////////////////////////
void 
BookDir::copy(std::istream& is, const Path& o)
{
	// really?! binary!
	// std::ofstream os(o.string(), std::ios::binary);
	// if (not os.good())
	// 	throw std::system_error(errno, std::system_category(), o.string());
	// os << is.rdbuf();
	// BOOST_LOG_TRIVIAL(info) << "(BookDir::copy) wrote " << os.tellp() 
	// 			<< " to " << o;
	// os.close();
}

////////////////////////////////////////////////////////////////////////////////
std::string
BookDir::get_hex_str(int id)
{
	// std::stringstream os;
	// os << std::hex << std::setw(10) << std::setfill('0') << id;
	// return os.str();	
	return {};
}
