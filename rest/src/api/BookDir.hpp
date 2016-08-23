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
		Path get_page(int id) const;
		static void copy(std::istream& is, const Path& o);
		static std::string get_hex_str(int id);
		const Path path_;
	};
}

#endif // pcw_BookDir_hpp__

