#ifndef pcw_BookParser_hpp__
#define pcw_BookParser_hpp__

#include <memory>

namespace pcw {
	class Book;
	using BookPtr = std::shared_ptr<Book>;

	class BookParser {
	public:
		virtual ~BookParser() noexcept = default;
		virtual BookPtr parse() = 0;
	};
}

#endif // pcw_BookParser_hpp__
