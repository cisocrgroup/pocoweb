#ifndef pcw_OcropusLlocsParserLine_hpp__
#define pcw_OcropusLlocsParserLine_hpp__

#include "core/Box.hpp"
#include "core/Line.hpp"
#include "ParserPage.hpp"

namespace pcw {
	class OcropusLlocsParserLine: public ParserLine {
	public:
		OcropusLlocsParserLine(int id, Path llocs, const Path& img);

		virtual ~OcropusLlocsParserLine() noexcept override = default;
		virtual void insert(size_t i, wchar_t c) override;
		virtual void erase(size_t i) override;
		virtual void set(size_t i, wchar_t c) override;
		virtual std::wstring wstring() const override;
		virtual std::string string() const override;
		virtual Line line(int id) const override;

	private:
		void init();

		Line line_;
		Path llocs_;
	};
}

#endif // pcw_OcropusLlocsParserLine_hpp__
