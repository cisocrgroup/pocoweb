#ifndef pcw_Book_hpp__
#define pcw_Book_hpp__

#include "Page.hpp"
#include "BookData.hpp"

namespace sql {
	class Connection;
}

namespace pcw {
	class Book: public std::enable_shared_from_this<Book>,
		    public Container<Page> {
	public:
		Book(int id);
		virtual ~Book() noexcept override = default;	
		PagePtr add_page(PagePtr page);
		PagePtr get_page(int id) const noexcept;
		void dbstore(sql::Connection& c) const;
		void dbload(sql::Connection& c);
		void load(nlohmann::json& json);
		void store(nlohmann::json& json) const;
		BookData data;
		int id;
	
	private:
		int first_page_id() const noexcept;
		int last_page_id() const noexcept;
	};
	using BookPtr = std::shared_ptr<Book>;
}

#endif // pcw_Book_hpp__
