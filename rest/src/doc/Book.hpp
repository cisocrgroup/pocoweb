#ifndef pcw_Book_hpp__
#define pcw_Book_hpp__

namespace pcw {

	struct BookData {
		std::string title, author, desc, uri, path;
		int id, year, firstpage, lastpage;
	};

	class Page;
	class Book: public std::enable_shared_from_this<Book>,
		    public Container<Page> {
	public:
		virtual ~Book() noexcept override = default;	
		BookData data;
		int id;
	};
	using BookPtr = std::shared_ptr<Book>;
}

#endif // pcw_Book_hpp__
