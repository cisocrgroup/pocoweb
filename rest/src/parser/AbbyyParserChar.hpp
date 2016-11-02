#ifndef pcw_AbbyyParserChar_hpp__
#define pcw_AbbyyParserChar_hpp__

#include <pugixml.hpp>
#include "ParserChar.hpp"

namespace pcw {
	class AbbyyParserChar: public BasicParserChar {
	public:
		AbbyyParserChar(const pugi::xml_node& node);
		virtual ~AbbyyParserChar() noexcept override = default;
		virtual void set(wchar_t) override;
		virtual void remove() override;
		virtual ParserCharPtr clone() const override;

	private:
		static wchar_t get_char_from_node(const pugi::xml_node& node);
		static void set_char_to_node(pugi::xml_node& node, wchar_t c);

		pugi::xml_node node_;
	};
}


#endif // pcw_AbbyyParserChar_hpp__

