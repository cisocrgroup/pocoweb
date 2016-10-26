#ifndef pcw_BookFixer_hpp__
#define pcw_BookFixer_hpp__

#include <memory>
#include <boost/filesystem/path.hpp>
#include <stdexcept>

namespace pcw {
	class Book;
	class Page;
	class Line;
	struct Config;
	using PagePtr = std::shared_ptr<Page>;
	
	class BookFixer {
	public:
		using Path = boost::filesystem::path;
		using Paths = std::vector<Path>;
		
		BookFixer(Paths imgs);
	
		void fix(Book& book) const;

	private:
		void fix_page_and_line_ordering(Book& book) const;
		void fix_image_paths(Book& book) const;
		void fix_image_paths(Page& page) const;
		void update_indizes(Book& book) const;

		const Paths imgs_;
	};
}

#endif // pcw_BookFixer_hpp__

