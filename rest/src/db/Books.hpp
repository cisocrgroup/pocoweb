#ifndef pcw_Books_hpp__
#define pcw_Books_hpp__

#include <memory>

namespace  sql {
	class Statement;
}

namespace pcw {
	class Session;
	using SessionPtr = std::shared_ptr<Session>;
	class Book;
	using BookPtr = std::shared_ptr<Book>;
	class Page;
	using PagePtr = std::shared_ptr<Page>;

	class Books {
	public:
		Books(SessionPtr session);
		BookPtr new_book(
			const std::string& title,
			const std::string& author,
			const std::string& dir
		) const;	
		BookPtr find_book(int bookid) const;
		bool is_allowed(const Book& book) const;
		bool is_owner(const Book& book) const;
		void insert_page(const Page& page) const;

	private:
		static int last_insert_id(sql::Statement& s);

		SessionPtr session_;
	};
}

#endif // pcw_Books_hpp__
