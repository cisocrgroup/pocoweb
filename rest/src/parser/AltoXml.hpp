#ifndef pcw_AltoXml_hpp__
#define pcw_AltoXml_hpp__

#include <pugixml.hpp>
#include "ParserChar.hpp"
#include "ParserWord.hpp"

namespace pcw {
	class AltoXmlParserWord;
	using AltoXmlParserWordPtr = std::shared_ptr<AltoXmlParserWord>;
	class AltoXmlSpaceChar;
	using AltoXmlSpaceCharPtr = std::shared_ptr<AltoXmlSpaceChar>;
	class AltoXmlChar;
	using AltoXmlCharPtr = std::shared_ptr<AltoXmlChar>;
	class ParserChar;
	using ParserCharPtr = std::shared_ptr<ParserChar>;
	class ParserLine;

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

	class AltoXmlChar: public ParserWordChar {
	public:
		virtual ~AltoXmlChar() noexcept override = default;
	};

	class AltoXmlParserWord: public ParserWord {
	public:
		AltoXmlParserWord(const pugi::xml_node& node);
		void add_chars_to_line(ParserLine& line);

	protected:
		virtual void update(const std::string& word) override;
		virtual void remove() override;
		virtual ParserWordPtr create() override;

	private:
		pugi::xml_node node_;
	};
}

#endif // pcw_AltoXml_hpp__
