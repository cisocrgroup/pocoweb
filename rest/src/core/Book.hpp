#ifndef pcw_Book_hpp__
#define pcw_Book_hpp__

#include <boost/filesystem/path.hpp>
#include <memory>
#include <vector>
#include "Project.hpp"

namespace pcw {
class User;
using ConstUserSptr = std::shared_ptr<const User>;
class Book;
using BookPtr = std::shared_ptr<Book>;
using BookSptr = std::shared_ptr<Book>;
using Path = boost::filesystem::path;

struct BookData {
	std::string author, title, description, uri, lang;
	Path dir;
	int year;
};

class Book : public Project {
       public:
	Book(int id = 0) : Project(id), data{}, owner_() {}
	Book(const Book& other) = delete;
	Book& operator=(const Book& other) = delete;
	Book(Book&& other) = delete;
	Book& operator=(Book&& other) = delete;
	virtual ~Book() noexcept override = default;
	virtual const Book& origin() const noexcept override { return *this; }
	virtual const User& owner() const noexcept override { return *owner_; }
	virtual void set_owner(const User& user) override;

	BookData data;

       private:
	ConstUserSptr owner_;
};
}

#endif  // pcw_Book_hpp__
