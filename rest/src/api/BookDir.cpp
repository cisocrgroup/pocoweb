#include <boost/filesystem/operations.hpp>
#include <fstream>
#include <sstream>
#include "doc/Book.hpp"
#include "doc/hocr.hpp"
#include "BookDir.hpp"

////////////////////////////////////////////////////////////////////////////////
pcw::BookDir::BookDir(const Book& book)
	: path_(book.data.path)
{
	boost::filesystem::create_directory(path_);	
}

////////////////////////////////////////////////////////////////////////////////
pcw::PagePtr
pcw::BookDir::parse_page_xml(int id) const
{
	auto file = get_page(id);
	file.replace_extension("xml");
	return parse_hocr_page(file);
}

////////////////////////////////////////////////////////////////////////////////
void 
pcw::BookDir::add_page_xml(int id, std::istream& is) const 
{
	auto ofile = get_page(id);
	ofile.replace_extension("xml");
	copy(is, ofile);
}

////////////////////////////////////////////////////////////////////////////////
void 
pcw::BookDir::add_page_image(int id, std::istream& is) const 
{
	auto ofile = get_page(id);
	ofile.replace_extension("png");
	copy(is, ofile);
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
	std::ofstream os(o.string());
	if (not os.good())
		throw std::system_error(errno, std::system_category(), o.string());
	is >> std::noskipws;
	std::copy(
		std::istream_iterator<char>(is), 
		std::istream_iterator<char>(), 
		std::ostream_iterator<char>(os)
	);
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
