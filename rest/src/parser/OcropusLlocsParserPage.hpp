#ifndef pcw_OcropusLlocsParserPage_hpp__
#define pcw_OcropusLlocsParserPage_hpp__

#include <memory>
#include "BasicParserPage.hpp"

namespace pcw {
	class OcropusLlocsParserPage: public BasicParserPage {
	public:
		OcropusLlocsParserPage(int id): id_(id) {}
		virtual ~OcropusLlocsParserPage() noexcept override = default;
		virtual void write(const Path& path) const override;

	private:
		void write(int id, const ParserLine& line, const Path& path) const;
		int id_;
	};
}

#endif // pcw_OcropusLlocsParserPage_hpp__


