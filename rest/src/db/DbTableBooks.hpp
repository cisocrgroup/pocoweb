#ifndef pcw_DbTableBooks_hpp__
#define pcw_DbTableBooks_hpp__

namespace pcw {
	class User;
	class Book;
	class Page;
	class Line;
	using BookPtr = std::shared_ptr<Book>;
	using PagePtr = std::shared_ptr<Page>;
	using LinePtr = std::shared_ptr<Line>;

	class DbTableBooks {
	public:
		DbTableBooks(ConnectionPtr conn);
		BookPtr insertBook(const User& owner, Book& book) const;
		PagePtr getPage(const User& user, int bookid, int pageid) const;

	private:
		void insertPage(const Book& book, const Page& page) const;
		void insertLine(const Book& book, 
				const Page& page, 
				const Line& line) const;
		static std::string cutsToString(const Line& line);
		const ConnectionPtr conn_;
	};
}

#endif // pcw_DbTableBooks_hpp__
