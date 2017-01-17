#ifndef pcw_BookBuilder_hpp__
#define pcw_BookBuilder_hpp__

#include <boost/filesystem/path.hpp>
#include <memory>

namespace pcw {
	class Book;
	using BookSptr = std::shared_ptr<Book>;
	class User;
	using UserSptr = std::shared_ptr<User>;
	using Path = boost::filesystem::path;

	class BookBuilder {
	public:
		BookBuilder();
		BookBuilder& reset();
		const BookBuilder& set_author(std::string author) const;
		const BookBuilder& set_title(std::string title) const;
		const BookBuilder& set_description(std::string desc) const;
		const BookBuilder& set_uri(std::string uri) const;
		const BookBuilder& set_language(std::string lang) const;
		const BookBuilder& set_directory(Path dir) const;
		const BookBuilder& set_year(int year) const;
		const BookBuilder& set_owner(UserSptr owner) const;
		const BookBuilder& set_id(int id) const;
		BookSptr build() const;

	private:
		BookSptr book_;
	};
}
#endif // pcw_BookBuilder_hpp__
