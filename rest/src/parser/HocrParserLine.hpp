#ifndef pcw_HocrParserLine_hpp__
#define pcw_HocrParserLine_hpp__

#include <pugixml.hpp>
#include "core/Box.hpp"
#include "ParserPage.hpp"

namespace pcw {
	class HocrParserLine: public ParserLine {
	public:
		using Node = pugi::xml_node;

		HocrParserLine(Node node);

		virtual ~HocrParserLine() noexcept override = default;
		virtual void end_wagner_fischer() override;
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
				, is_space(isspace(cc))
			{}
			Node node;
			bool is_space;
		};
		using Chars = std::vector<Char>;
		using Iterator = Chars::iterator;

		void update_char(Iterator b, Iterator e, Node& parent);
		void init_string(const Node& node, bool space_before);
		void init_space(const Node& node);
		void init();

		static void set_box(const Box& box, Node& node);
		static void set_conf(double conf, Node& node);
		static void set_content(const std::wstring& str, Node& node);
		static Iterator find_end_of_token(Iterator b, Iterator e) noexcept;
		static Char make_copy(Char& c, wchar_t cc);
		static Node merge(Node& a, const Node& b);

		std::vector<Char> chars_;
		Node node_;
		bool needs_update_;
	};	
}

#endif // pcw_HocrParserLine_hpp__
