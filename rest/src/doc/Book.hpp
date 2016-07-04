#ifndef pcw_Book_hpp__
#define pcw_Book_hpp__

namespace pcw {

	class Page;
	struct BookData;

	class Book: public std::enable_shared_from_this<Book>,
		    public Container<Page> {
	public:
		Book() = default;
		Book(nlohmann::json& json): Book() {load(json);}
		virtual ~Book() noexcept override = default;	
		void load(nlohmann::json& json);
		void store(nlohmann::json& json) const;
		BookData data;
		int id;
	};
	using BookPtr = std::shared_ptr<Book>;
}

#endif // pcw_Book_hpp__
