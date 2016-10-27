#ifndef pcw_Book_hpp__
#define pcw_Book_hpp__

#include <boost/filesystem/path.hpp>
#include <memory>
#include <vector>
#include "Project.hpp"

namespace pcw {
	class User;
	using ConstUserPtr = std::shared_ptr<const User>;
	class Book;
	using BookPtr = std::shared_ptr<Book>;
	using Path = boost::filesystem::path;

	class Book: public Project {
	public:
		Book(int id = 0)
			: author()
			, title()
			, description()
			, uri()
			, dir()
			, year()
			, owner_()
			, id_(id) 
		{}
		Book(const Book& other) = delete;
		Book& operator=(const Book& other) = delete;
		Book(Book&& other) = delete;
		Book& operator=(Book&& other) = delete;
		virtual ~Book() noexcept override = default;
		virtual const Book& origin() const noexcept override {return *this;}
		virtual const User& owner() const noexcept override {return *owner_;}
		virtual int id() const noexcept override {return id_;}
		void set_id(int id) noexcept {assert(not id_); id_ = id;}
		void set_owner(const User& user);
		void push_back(PagePtr page);

		
		std::string author, title;
		std::string description, uri;
		Path dir;
		int year;

	private:
		ConstUserPtr owner_;
		int id_;
	};
}

#endif // pcw_Book_hpp__
