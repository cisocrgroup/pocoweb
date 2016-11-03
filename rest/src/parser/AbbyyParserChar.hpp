#ifndef pcw_AbbyyParserChar_hpp__
#define pcw_AbbyyParserChar_hpp__

#include <pugixml.hpp>
#include "ParserChar.hpp"

namespace pcw {
	class AbbyyParserChar: public ParserChar {
	public:
		AbbyyParserChar(const pugi::xml_node& node);
		virtual ~AbbyyParserChar() noexcept override = default;
		virtual void set(wchar_t c) override;
		virtual void remove() override;
		virtual ParserCharPtr clone() override;

	private:
		static double get_conf_from_node(const pugi::xml_node& node);
		static Box get_box_from_node(const pugi::xml_node& node);
		static void set_box_to_node(pugi::xml_node& node, const Box& box);
		static wchar_t get_char_from_node(const pugi::xml_node& node);
		static void set_char_to_node(pugi::xml_node& node, wchar_t c);

		pugi::xml_node node_;
	};
}


#endif // pcw_AbbyyParserChar_hpp__

