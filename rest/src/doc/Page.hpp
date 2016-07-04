#ifndef pcw_Page_hpp__
#define pcw_Page_hpp__

namespace pcw {
	class Line;
	struct Box;

	class Page: public std::enable_shared_from_this<Page>,
		    public Container<Line> {
	public:
		virtual ~Page() noexcept override = default;
		Box box;
		std::string image;
		int id;
	};
	using PagePtr = std::shared_ptr<Page>;
}

#endif // pcw_Page_hpp__

