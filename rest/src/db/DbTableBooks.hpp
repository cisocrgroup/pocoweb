#ifndef pcw_DbTableBooks_hpp__
#define pcw_DbTableBooks_hpp__

namespace pcw {
	class User;
	class Book;
	class Page;
	using BookPtr = std::shared_ptr<Book>;
	using PagePtr = std::shared_ptr<Page>;

	class DbTableBooks {
	public:
		DbTableBooks(ConnectionPtr conn);
		BookPtr insertBook(const User& owner, Book& book) const;
		PagePtr insertBook(const Book& book, Page& page) const;

	private:
		const ConnectionPtr conn_;
	};
}

#endif // pcw_DbTableBooks_hpp__
