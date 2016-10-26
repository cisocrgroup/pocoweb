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
	class Page;
	using PagePtr = std::shared_ptr<Page>;
	using Path = boost::filesystem::path;

	class Book: private std::vector<PagePtr>,
		    public Project {
	public:
		using Base = std::vector<PagePtr>;
		using value_type = Base::value_type;
	
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
		virtual void set_owner(const User& user) override;
		virtual void each_page(Callback f) const override;

		using Base::begin;
		using Base::end;
		using Base::back;
		using Base::front;
		using Base::empty;
		using Base::size;
		using Base::operator[];
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
