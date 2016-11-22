#ifndef pcw_Book_hpp__
#define pcw_Book_hpp__

#include <boost/filesystem/path.hpp>
#include <memory>
#include <vector>
#include "BookView.hpp"

namespace pcw {
	class User;
	using ConstUserPtr = std::shared_ptr<const User>;
	class Book;
	using BookPtr = std::shared_ptr<Book>;
	using Path = boost::filesystem::path;

	class Book: public BookView {
	public:
		Book(int id = 0)
			: BookView(id)
			, author()
			, title()
			, description()
			, uri()
			, dir()
			, year()
			, owner_()
		{}
		Book(const Book& other) = delete;
		Book& operator=(const Book& other) = delete;
		Book(Book&& other) = delete;
		Book& operator=(Book&& other) = delete;
		virtual ~Book() noexcept override = default;
		virtual const Book& origin() const noexcept override {return *this;}
		virtual const User& owner() const noexcept override {return *owner_;}
		void set_owner(const User& user);

		std::string author, title, description, uri;
		Path dir;
		int year;

	private:
		ConstUserPtr owner_;
	};
}

#endif // pcw_Book_hpp__
