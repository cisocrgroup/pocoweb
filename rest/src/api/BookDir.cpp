#include <boost/filesystem/operations.hpp>
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

}
