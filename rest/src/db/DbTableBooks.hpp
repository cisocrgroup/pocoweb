#ifndef pcw_DbTableBooks_hpp__
#define pcw_DbTableBooks_hpp__

namespace pcw {
	class User;
	class Book;
	class BookData;
	class Page;
	class Line;
	using BookPtr = std::shared_ptr<Book>;
	using PagePtr = std::shared_ptr<Page>;
	using LinePtr = std::shared_ptr<Line>;

	class DbTableBooks {
	public:
		DbTableBooks(ConnectionPtr conn);
		BookPtr insertBook(int userid, Book& book) const;
		std::vector<BookPtr> getAllowedBooks(int userid) const;
		void allowBook(int userid, int bookid) const;
		void denyBook(int userid, int bookid) const;
		bool pageIsAllowed(int userid, int bookid, int pageid) const;
		BookPtr get(int bookid, int pageid, int lineid) const;

	private:
		void insertPage(const Book& book, const Page& page) const;
		void insertLine(const Book& book, 
				const Page& page, 
				const Line& line) const;
		BookPtr getLine(int bookid, int pageid, int lineid) const;
		BookPtr getPage(int bookid, int pageid) const;
		BookPtr getBook(int bookid) const;

		static BookPtr makeBook(sql::ResultSet& res);
		static BookPtr doMakeBook(sql::ResultSet& res);
		static PagePtr doMakePage(sql::ResultSet& res);
		static LinePtr doMakeLine(sql::ResultSet& res);
		static std::string cutsToString(const Line& line);
		int insertBookData(int owner, const BookData& book) const;
		int insertBook(const Book& book) const;
		const ConnectionPtr conn_;
	};
}

#endif // pcw_DbTableBooks_hpp__
