#ifndef pcw_AltoXmlParserWord_hpp__
#define pcw_AltoXmlParserWord_hpp__

#include <pugixml.hpp>
#include "core/Box.hpp"
#include "ParserWord.hpp"

namespace pcw {
	class ParserLine;
	class ParserChar;
	using ParserCharPtr = std::shared_ptr<ParserChar>;

	class AltoXmlParserWord: public ParserWord {
	public:
		AltoXmlParserWord(const pugi::xml_node& node);
		void add_chars_to_line(ParserLine& line);

	protected:
		virtual void update(const std::string& word) override;
		virtual void remove() override;

	private:
		pugi::xml_node node_;
	};
}

#endif // pcw_AltoXmlParserWord_hpp__
