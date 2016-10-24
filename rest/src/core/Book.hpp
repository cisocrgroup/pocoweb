#ifndef pcw_Book_hpp__
#define pcw_Book_hpp__

#include <boost/filesystem/path.hpp>
#include <memory>
#include <vector>

namespace pcw {
	class User;
	using UserPtr = std::shared_ptr<User>;
	class Book;
	using BookPtr = std::shared_ptr<Book>;
	class Page;
	using PagePtr = std::shared_ptr<Page>;
	using Path = boost::filesystem::path;

	class Book: private std::vector<PagePtr>,
		    public std::enable_shared_from_this<Book> {
	public:
		using Base = std::vector<PagePtr>;
		using value_type = Base::value_type;
	
		Book()
			: owner()
			, author()
			, title()
			, description()
			, uri()
			, dir()
			, id() 
			, year()
		{}

		using Base::begin;
		using Base::end;
		using Base::back;
		using Base::front;
		using Base::empty;
		using Base::size;
		void push_back(PagePtr page);
		
		UserPtr owner;
		std::string author, title;
		std::string description, uri;
		Path dir;
		int id;
		int year;
	};
}

#endif // pcw_Book_hpp__
