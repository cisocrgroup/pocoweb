#include <boost/filesystem/operations.hpp>
#include <boost/log/trivial.hpp>
#include <fstream>
#include <sstream>
#include "doc/Book.hpp"
#include "util/Pix.hpp"
#include "BookDir.hpp"

////////////////////////////////////////////////////////////////////////////////
pcw::BookDir::BookDir(const Book& book)
	: path_(book.data.path)
{
	boost::filesystem::create_directory(path_);	
}

////////////////////////////////////////////////////////////////////////////////
void 
pcw::BookDir::add_page_ocr(Page& page, std::istream& is) const 
{
	page.ocrfile = get_page(page.id);
	page.ocrfile.replace_extension("xml");
	BOOST_LOG_TRIVIAL(debug) << "(BookDir::add_page_ocr) ofile: " << page.ocrfile;
	copy(is, page.ocrfile);
	page.parse();
	add_line_images(page);
}

////////////////////////////////////////////////////////////////////////////////
pcw::PagePtr 
pcw::BookDir::add_page_image(int id, const std::string& ext, std::istream& is) const 
{
	auto page = std::make_shared<Page>();
	page->id = id;
	page->imagefile = get_page(id);
	page->imagefile.replace_extension(ext);
	BOOST_LOG_TRIVIAL(debug) << "(BookDir::add_page_image) ofile: " << page->imagefile;
	copy(is, page->imagefile);
	return page;
}

// #define VAR(x) #x << "=" << x

////////////////////////////////////////////////////////////////////////////////
void 
pcw::BookDir::add_line_images(Page& page) const
{
	auto dir = get_page(page.id);
	boost::filesystem::create_directory(dir);
	PixPtr pix{pixRead(page.imagefile.string().data())};
	
	for (auto& line: page) {
		if (line)
			add_line_image(*line, dir, pix.get());
	}
}

////////////////////////////////////////////////////////////////////////////////
void 
pcw::BookDir::add_line_image(Line& line, const Path& dir, void *ppix) const
{
	// TODO: remove void*
	auto file = dir / ("line-" + get_hex_str(line.id));
	file.replace_extension("png");	
	line.image = file.string();
	
	auto pix = static_cast<PIX*>(ppix);
	if (pix) {
		// only works iff the same order of the struct's definition is used
		BOX box {
			.x = line.box.left,
			.y = line.box.top,
			.w = line.box.width(),
			.h = line.box.height(),
			.refcount = 0 
		};
		PixPtr tmp{pixClipRectangle(pix, &box, nullptr)};
		if (tmp) 
			pixWrite(line.image.data(), tmp.get(), IFF_PNG);
	}
}

////////////////////////////////////////////////////////////////////////////////
pcw::BookDir::Path
pcw::BookDir::get_page(int id) const
{
	return path_ / ("page-" + get_hex_str(id));
}

////////////////////////////////////////////////////////////////////////////////
void 
pcw::BookDir::copy(std::istream& is, const Path& o)
{
	// really?! binary!
	std::ofstream os(o.string(), std::ios::binary);
	if (not os.good())
		throw std::system_error(errno, std::system_category(), o.string());
	os << is.rdbuf();
	BOOST_LOG_TRIVIAL(info) << "(BookDir::copy) wrote " << os.tellp() 
				<< " to " << o;
	os.close();
}

////////////////////////////////////////////////////////////////////////////////
std::string
pcw::BookDir::get_hex_str(int id)
{
	std::stringstream os;
	os << std::hex << std::setw(10) << std::setfill('0') << id;
	return os.str();	
}
