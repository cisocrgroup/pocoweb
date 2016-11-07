#ifndef pcw_AltoXmlSpaceChar_hpp__
#define pcw_AltoXmlSpaceChar_hpp__

#include <pugixml.hpp>
#include "ParserChar.hpp"

namespace pcw {
	class AltoXmlParserWord;
	using AltoXmlParserWordPtr = std::shared_ptr<AltoXmlParserWord>;
	class AltoXmlSpaceChar;
	using AltoXmlSpaceCharPtr = std::shared_ptr<AltoXmlSpaceChar>;

	class AltoXmlSpaceChar: public ParserChar {
	public:
		AltoXmlSpaceChar(const pugi::xml_node& node);
		virtual ~AltoXmlSpaceChar() noexcept override = default;
		virtual ParserCharPtr set(wchar_t c) override;
		virtual void remove() override;
		virtual ParserCharPtr insert(wchar_t c) override;
		void set_left(AltoXmlParserWordPtr left) noexcept {left_ = std::move(left);}
		void set_right(AltoXmlParserWordPtr right) noexcept {right_ = std::move(right);}
		
	private:
		ParserWordCharPtr new_token(wchar_t c);
		double conf() const;

		pugi::xml_node node_;
		AltoXmlParserWordPtr left_, right_;
	};
}

#endif // pcw_AltoXmlSpaceChar_hpp__
