#ifndef pcw_Document_hpp__
#define pcw_Document_hpp__

namespace pcw {
	class Line;
	class Page;
	class Book;
	using LinePtr = std::shared_ptr<Line>;
	using PagePtr = std::shared_ptr<Page>;
	using BookPtr = std::shared_ptr<Book>;

	struct Box {
		int x0, y0, x1, y1;
	};

	template<class T>
	class Container {
	public:
		using Ptr = std::shared_ptr<T>;
		using Ts = std::vector<Ptr>;
		using const_iterator = Ts::const_iterator;
		using iterator = Ts::iterator;
		using value_type = Ts::value_type;

		virtual ~Container() noexcept = default;
		size_t size() const noexcept {return ts_.size();}
		const Ptr& operator[](size_t i) const {return ts_.at(i);}
		Ptr& operator[](size_t i) {return ts_[i];}
		void resize(size_t n) {ts_.resize(n);}

		auto begin() const noexcept {return ts_.begin();}
		auto end() const noexcept {return ts_.end();}
		auto begin() noexcept {return ts_.begin();}
		auto end() noexcept {return ts_.end();}

	private:
		Ts ts_;
	};

	class Book: public enable_shared_from_this<Book>,
		    public Container<Page> {
	public:
		virtual ~Book() noexcept override = default;	
	};

	class Page: public enable_shared_from_this<Page>,
		    public Container<Line> {
	public:
		virtual ~Page() noexcept override = default;
		Box box;
	};

	class Line: public enable_shared_from_this<Line> {
	public:
		using Cuts = std::vector<int>;

		virtual ~Line() noexcept override = default;
		const std::string& line() const noexcept {return line_;}
		std::string& line() noexcept {return line_;}
		const Cuts& cuts() const noexcept {return cuts_;}
		Cuts& cuts() noexcept {return cuts_;}
		Box box;

	private:
		std::string line_;
		Cuts cuts_;
	};
}

#endif // pcw_Document_hpp__
