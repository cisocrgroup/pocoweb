#ifndef pcw_AltoXmlParserWord_hpp__
#define pcw_AltoXmlParserWord_hpp__

#include <pugixml.hpp>
#include "ParserWord.hpp"

namespace pcw {
	class ParserLine;
	class ParserChar;
	using ParserCharPtr = std::shared_ptr<ParserChar>;

	class AltoXmlParserWord: public ParserWord {
	public:
		AltoXmlParserWord(const pugi::xml_node& node);
		virtual void update() override;
		virtual void remove() override;
		void add_chars_to_line(ParserLine& line);

	private:
		pugi::xml_node node_;
	};
}

#endif // pcw_AltoXmlParserWord_hpp__
