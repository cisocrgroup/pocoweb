#ifndef pcw_PageParser_hpp__
#define pcw_PageParser_hpp__

#include <memory>

namespace pcw {
	class ParserPage;
	using ParserPagePtr = std::shared_ptr<ParserPage>;

	class PageParser {
	public:
		virtual ~PageParser() noexcept = default;
		virtual bool has_next() const noexcept = 0;
		virtual ParserPagePtr parse() = 0;
	};
}

#endif // pcw_PageParser_hpp__
