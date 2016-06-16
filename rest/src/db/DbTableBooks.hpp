#ifndef pcw_DbTableBooks_hpp__
#define pcw_DbTableBooks_hpp__

namespace pcw {
	class User;
	class Book;
	using BookPtr = std::shared_ptr<Book>;

	class DbTableBooks {
	public:
		DbTableBooks(ConnectionPtr conn);
		BookPtr insertBook(const User& owner, Book& book) const;

	private:
		const ConnectionPtr conn_;
	};
}

#endif // pcw_DbTableBooks_hpp__
