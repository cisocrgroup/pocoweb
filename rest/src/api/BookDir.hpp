#ifndef pcw_BookDir_hpp__
#define pcw_BookDir_hpp__

#include <boost/filesystem/path.hpp>

namespace pcw {
	class Book;

	class BookDir {
	public:
		BookDir(const Book& book);
		void add_page_image(int id, std::istream& is);

	private:
		using Path = boost::filesystem::path;
		const Path path_;
	};
}

#endif // pcw_BookDir_hpp__

