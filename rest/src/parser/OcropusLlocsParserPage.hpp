#ifndef pcw_OcropusLlocsParserPage_hpp__
#define pcw_OcropusLlocsParserPage_hpp__

#include <memory>
#include "BasicParserPage.hpp"

namespace pcw {
class OcropusLlocsParserPage : public BasicParserPage {
       public:
	OcropusLlocsParserPage(int id);
	virtual ~OcropusLlocsParserPage() noexcept override = default;
	virtual void write(const Path& path) const override;
	const Path& dir() const noexcept { return dir_; }

       private:
	static void write(int id, const ParserLine& line, const Path& base);
	static void copy(const Path& from, const Path& to);

	Path dir_;
};
}

#endif  // pcw_OcropusLlocsParserPage_hpp__
