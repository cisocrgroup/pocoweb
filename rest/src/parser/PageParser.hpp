#ifndef pcw_PageParser_hpp__
#define pcw_PageParser_hpp__

#include <memory>

namespace pcw {
	class Page;
	using PagePtr = std::shared_ptr<Page>;

	class PageParser {
	public:
		virtual ~PageParser() noexcept = default;
		virtual bool has_next() const noexcept = 0;
		virtual PagePtr parse() = 0;
	};
}

#endif // pcw_PageParser_hpp__
