#include <boost/filesystem/operations.hpp>
#include <fstream>
#include <sstream>
#include "doc/Book.hpp"
#include "BookDir.hpp"

////////////////////////////////////////////////////////////////////////////////
pcw::BookDir::BookDir(const Book& book)
	: path_(book.data.path)
{
	boost::filesystem::create_directory(path_);	
}

////////////////////////////////////////////////////////////////////////////////
void 
pcw::BookDir::add_page_image(int id, std::istream& is)
{
	auto ofile = get_page(id);
	ofile.replace_extension("img");
	std::ofstream os(ofile.string());
	if (not os.good())
		throw std::system_error(errno, std::system_category(), ofile.string());

	is >> std::noskipws;
	std::copy(
		std::istream_iterator<char>(is), 
		std::istream_iterator<char>(), 
		std::ostream_iterator<char>(os)
	);
	os.close();
}

////////////////////////////////////////////////////////////////////////////////
pcw::BookDir::Path
pcw::BookDir::get_page(int id) const
{
	return path_ / ("page-" + get_hex_str(id));
}

////////////////////////////////////////////////////////////////////////////////
std::string
pcw::BookDir::get_hex_str(int id)
{
	std::stringstream os;
	os << std::hex << std::setw(10) << std::setfill('0') << id;
	return os.str();	
}
