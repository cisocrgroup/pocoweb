#ifndef pcw_AltoXmlParserWord_hpp__
#define pcw_AltoXmlParserWord_hpp__

#include <pugixml.hpp>
#include "ParserWord.hpp"

namespace pcw {
	class ParserChar;
	using ParserCharPtr = std::shared_ptr<ParserChar>;
	using Chars = std::vector<ParserCharPtr>;

	class AltoXmlParserWord: public ParserWord {
	public:
		AltoXmlParserWord(const pugi::xml_node& node, Chars& chars);
		virtual void update() override;
		virtual void remove() override;

	private:
		void make(Chars& chars);
		pugi::xml_node node_;
	};
}

#endif // pcw_AltoXmlParserWord_hpp__
