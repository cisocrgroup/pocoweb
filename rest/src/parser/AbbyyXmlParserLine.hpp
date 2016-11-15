#ifndef pcw_AbbyyXmlParserLine_hpp__
#define pcw_AbbyyXmlParserLine_hpp__

#include <pugixml.hpp>
#include "core/Box.hpp"
#include "ParserPage.hpp"

namespace pcw {
	class AbbyyXmlParserLine: public ParserLine {
	public:
		using Node = pugi::xml_node;

		AbbyyXmlParserLine(Node node);

		virtual ~AbbyyXmlParserLine() noexcept override = default;
		virtual void insert(size_t i, wchar_t c) override;
		virtual void erase(size_t i) override;
		virtual void set(size_t i, wchar_t c) override;
		virtual std::wstring wstring() const override;
		virtual std::string string() const override;
		virtual Line line(int id) const override;

	private:
		struct Char: public ParserLine::Char {
			Char(wchar_t cc, Node nnode, double cconf, Box bbox)
				: ParserLine::Char(cc, cconf, bbox)
				, node(nnode)
			{}
			Node node;
		};
		void init();

		static Box get_box(const Node& node);
		static void set_box(const Box& box, Node& node);
		static wchar_t get_char(const Node& node);
		static void set_char(wchar_t c, Node& node);
		static Char char_before(Char& c, wchar_t cc);

		std::vector<Char> chars_;
		Node node_;
	};
}

#endif // pcw_AbbyyXmlParserLine_hpp__
