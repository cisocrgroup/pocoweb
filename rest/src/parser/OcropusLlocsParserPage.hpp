#ifndef pcw_OcropusLlocsParserPage_hpp__
#define pcw_OcropusLlocsParserPage_hpp__

#include <memory>
#include "BasicParserPage.hpp"

namespace pcw {
	class OcropusLlocsParserPage: public BasicParserPage {
	public:
		OcropusLlocsParserPage(Path dir): dir_(std::move(dir)) {}
		virtual ~OcropusLlocsParserPage() noexcept override = default;
		virtual void write(const Path& path) const override;

	private:
		void write(int id, const ParserLine& line, const Path& path) const;
		Path dir_;
	};
}

#endif // pcw_OcropusLlocsParserPage_hpp__


