#ifndef pcw_XmlPageParsing_hpp__
#define pcw_XmlPageParsing_hpp__

#include <boost/filesystem/path.hpp>
#include <iostream>

namespace pcw {
	class Book;
	size_t add_pages(const boost::filesystem::path& path, Book& book);
}

#endif // pcw_XmlPageParsing_hpp__
