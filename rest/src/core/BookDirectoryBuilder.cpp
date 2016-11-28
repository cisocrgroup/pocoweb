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
#include <crow/logging.h>
#include "util.hpp"
#include "Config.hpp"
#include "Error.hpp"
#include "Book.hpp"
#include "Page.hpp"
#include "Pix.hpp"
#include "BookDirectoryBuilder.hpp"
#include "parser/AltoXmlPageParser.hpp"
#include "parser/OcropusLlocsPageParser.hpp"
#include "parser/AbbyyXmlPageParser.hpp"
#include "parser/HocrPageParser.hpp"

namespace fs = boost::filesystem;
using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
static BookDirectoryBuilder::Path
create_unique_bookdir_path(const Config& config)
{
	BookDirectoryBuilder::Path path(config.daemon.basedir);
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
BookDirectoryBuilder::BookDirectoryBuilder(const Config& config)
	: BookDirectoryBuilder(create_unique_bookdir_path(config))
{
	assert(fs::is_directory(dir_));
}

////////////////////////////////////////////////////////////////////////////////
BookDirectoryBuilder::BookDirectoryBuilder(Path path)
	: dir_(path)
	, tmp_dir_(path / ".tmp")
	, line_img_dir_(path / "line-images")
{
	assert(fs::is_directory(dir_));
}

////////////////////////////////////////////////////////////////////////////////
BookDirectoryBuilder::~BookDirectoryBuilder() noexcept
{
	// CROW_LOG_INFO << "(BookDirectoryBuilder) Removing tmp dir " << tmp_dir();
	// clean up tmp directory; do not throw
	boost::system::error_code ec;
	fs::remove_all(tmp_dir(), ec);
	if (ec) {
		// CROW_LOG_WARNING << "(BookDirectoryBuilder) Could not remove "
				 // << tmp_dir() << ": " << ec.message();
	}
}

////////////////////////////////////////////////////////////////////////////////
void
BookDirectoryBuilder::remove() const
{
	// CROW_LOG_INFO << "(BookDirectoryBuilder) Removing directory " << dir_;
	fs::remove_all(dir_);
}

////////////////////////////////////////////////////////////////////////////////
void
BookDirectoryBuilder::add_zip_file(const std::string& content)
{
	auto tdir = tmp_dir();
	fs::create_directory(tdir);
	auto zip = zip_file();
	// CROW_LOG_DEBUG << "(BookDirectoryBuilder) content.size(): " << content.size();
	std::ofstream os(zip.string());
	if (not os.good())
		throw std::system_error(errno, std::system_category(), zip.string());
	os << content;
	os.close();
	std::string command = "unzip -qq -d " + tdir.string() + " " + zip.string();
	// CROW_LOG_DEBUG << "(BookDirectoryBuilder) Unzip command: " << command;
	auto err = system(command.data());
	if (err)
		THROW(Error, "Cannot unzip file: `", command, "` returned ", err);
	fs::recursive_directory_iterator i(tdir), e;
	for (; i != e; ++i) {
		add_file(*i);
	}
}

////////////////////////////////////////////////////////////////////////////////
void
BookDirectoryBuilder::add_file(const Path& path)
{
	builder_.add(path);
}

////////////////////////////////////////////////////////////////////////////////
BookPtr
BookDirectoryBuilder::build() const
{
	const auto book = builder_.build();
	assert(book);
	setup(*book);
	book->dir = dir_;
	return book;
}

////////////////////////////////////////////////////////////////////////////////
void
BookDirectoryBuilder::setup(const Book& book) const
{
	for (const auto& page: book) {
		assert(page);
		setup_img_and_ocr_files(*page);
		const auto pagedir = line_img_dir() / path_from_id(page->id());
		make_line_img_files(pagedir, *page);
	}
}

////////////////////////////////////////////////////////////////////////////////
void
BookDirectoryBuilder::setup_img_and_ocr_files(Page& page) const
{
	auto do_copy = [this](const auto& path) {
		auto to = dir_ / remove_common_base_path(path, tmp_dir());
		fs::create_directories(to.parent_path());
		copy(path, to);
		return to;
	};
	if (page.has_ocr_path()) {
		page.ocr = do_copy(page.ocr);
	}
	if (page.has_img_path()) {
		page.img = do_copy(page.img);
	}
}

////////////////////////////////////////////////////////////////////////////////
void
BookDirectoryBuilder::make_line_img_files(const Path& pagedir, Page& page) const
{
	PixPtr pix;
	if (page.has_img_path()) {
		pix.reset(pixRead(page.img.string().data()));
		if (not pix)
			THROW(Error, "(BookDirectoryBuilder) Cannot read img ", page.img);
	}
	for (auto& line: page) {
		if (not line->has_img_path() and not pix) {
			// CROW_LOG_WARNING << "(BookDirectoryBuilder) Missing image file for: "
					 // << page.ocr;
		} else if (not line->has_img_path() and pix) {
			line->img = pagedir / path_from_id(line->id());
			line->img.replace_extension(page.img.extension());
			fs::create_directories(pagedir);
			write_line_img_file(pix.get(), *line);
		} else if (line->has_img_path()) {
			auto to = dir_ / remove_common_base_path(line->img, tmp_dir());
			fs::create_directories(to.parent_path());
			copy(line->img, to);
			line->img = to;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
static void
clip(BOX& box, const PIX& pix)
{
	if (box.x + box.w > (int) pix.w) {
		int width = pix.w - box.x;
		// CROW_LOG_WARNING << "(BookDirectoryBuilder) Clipping box width from "
				 // << box.w << " to " << width;
		box.w = width;
	}
	if (box.y + box.h > (int) pix.h) {
		int height = pix.h - box.y;
		// CROW_LOG_WARNING << "(BookDirectoryBuilder) Clipping box height from "
				 // << box.h << " to " << height;
		box.h = height;
	}
}

////////////////////////////////////////////////////////////////////////////////
void
BookDirectoryBuilder::write_line_img_file(void *vpix, const Line& line)
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
	clip(box, *pix);
	if (box.x + box.w <= (int) pix->w and box.y + box.h <= (int) pix->h) {
		PixPtr tmp{pixClipRectangle(pix, &box, nullptr)};
		if (not tmp or pixWrite(line.img.string().data(), tmp.get(), format))
			THROW(Error, "(BookDirectoryBuilder) Cannot write img ", line.img);
	} else {
		// CROW_LOG_WARNING << "Cannot write line image for " << line.cor();
	}
}

////////////////////////////////////////////////////////////////////////////////
void
BookDirectoryBuilder::copy(const Path& from, const Path& to)
{
	// CROW_LOG_DEBUG << "(BookDirectoryBuilder) copying " << from << " to " << to;
	// fs::create_hard_link(to, from); TODO: ??!!
	boost::system::error_code ec;
	fs::create_hard_link(from, to, ec);
	if (ec) { // could not create hard link; try copy
		// CROW_LOG_WARNING << "Could not create hardlink from "
				 // << from << " to " << to << ": "
				 // << ec.message();
		fs::copy_file(from, to);
	}
}

////////////////////////////////////////////////////////////////////////////////
BookDirectoryBuilder::Path
BookDirectoryBuilder::path_from_id(int id)
{
	std::stringstream os;
	os << std::hex << std::setw(10) << std::setfill('0') << id;
	return os.str();
}

////////////////////////////////////////////////////////////////////////////////
Path
BookDirectoryBuilder::remove_common_base_path(const Path& p, const Path& base)
{
	auto i = std::mismatch(p.begin(), p.end(), base.begin(), base.end());
	if (i.first != p.end()) {
		Path res;
		for (auto j = i.first; j != p.end(); ++j) {
			res /= *j;
		}
		return res;
	}
	return p;
}
