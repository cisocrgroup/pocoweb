#include <boost/algorithm/string.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/log/trivial.hpp>
#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include "util.hpp"
#include "Config.hpp"
#include "BadRequest.hpp"
#include "BookFixer.hpp"
#include "Book.hpp"
#include "Page.hpp"
#include "Pix.hpp"
#include "BookDir.hpp"

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
			std::cerr << "OCR FILE: " << *i << "\n";
			//pcw::add_pages(*i, book);
		} else if (is_img_file(*i)) {
			imgs.push_back(*i);
		}
	}
	BookFixer book_fixer(std::move(imgs));
	book_fixer.fix(book);
	setup_directory_structure(book);
	clean_up(); // clean up tmpdir
}

////////////////////////////////////////////////////////////////////////////////
void
BookDir::setup_directory_structure(Book& book) const
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
	Path pagedir = path_ / path_from_id(page.id);
	fs::create_directory(pagedir);
	return pagedir;
}

////////////////////////////////////////////////////////////////////////////////
void
BookDir::copy_img_and_ocr_files(const Path& pagedir, Page& page) const
{
	// fs::create_hard_link(to, from);
	auto tocr = pagedir / page.ocr.filename();
	fs::create_hard_link(page.ocr, tocr);
	auto timg = pagedir/ page.img.filename();
	fs::create_hard_link(page.img, timg);
	page.ocr = tocr;
	page.img = timg;
}

////////////////////////////////////////////////////////////////////////////////
void
BookDir::make_line_img_files(const Path& pagedir, Page& page) const
{
	PixPtr pix{pixRead(page.img.string().data())};
	if (not pix)
		throw std::runtime_error("(BookDir) Cannot read image " + page.img.string());
	auto format = get_img_format(page.img);
	for (auto& line: page) {
		line.img = pagedir / path_from_id(line.id);
		line.img.replace_extension(page.img.extension());
		write_line_img_file(pix.get(), line, format);
	}
}

////////////////////////////////////////////////////////////////////////////////
void
BookDir::write_line_img_file(void *vpix, const Line& line, uint32_t format)
{
	auto pix = (PIX*)vpix;
	assert(pix);
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
bool
BookDir::is_ocr_file(const Path& path)
{
	return fs::is_regular_file(path) and path.extension() == ".xml";
}

////////////////////////////////////////////////////////////////////////////////
uint32_t 
BookDir::get_img_format(const Path& path) noexcept
{
	auto ext = path.extension().string();
	if (boost::iequals(ext, ".jpg") or boost::iequals(ext, "jpeg"))
		return IFF_JFIF_JPEG;
	if (boost::iequals(ext, ".png"))
		return IFF_PNG;
	if (boost::iequals(ext, ".tif") or boost::iequals(ext, "tiff"))
		return IFF_TIFF;
	return IFF_DEFAULT;
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
BookDir::Path
BookDir::path_from_id(int id)
{
	std::stringstream os;
	os << std::hex << std::setw(10) << std::setfill('0') << id;
	return os.str();
}
