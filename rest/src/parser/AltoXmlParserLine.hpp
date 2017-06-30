#ifndef pcw_AltoXmlParserLine_hpp__
#define pcw_AltoXmlParserLine_hpp__

#include "pugixml.hpp"
#include "core/Box.hpp"
#include "ParserPage.hpp"

namespace pcw {
	class AltoXmlParserLine: public ParserLine {
	public:
		using Node = pugi::xml_node;

		AltoXmlParserLine(Node node, bool explicit_spaces_ = true);

		virtual ~AltoXmlParserLine() noexcept override = default;
		virtual void end_wagner_fischer() override;
		virtual void insert(size_t i, wchar_t c) override;
		virtual void erase(size_t i) override;
		virtual void set(size_t i, wchar_t c) override;
		virtual std::wstring wstring() const override;
		virtual std::string string() const override;
		virtual LinePtr line(int id) const override;

	private:
		enum class Type {Char, Space, Hyphen};
		struct Char: public ParserLine::Char {
			Char(wchar_t cc, Node nnode, Type ttype, double cconf, Box bbox)
				: ParserLine::Char(cc, cconf, bbox)
				, node(nnode)
				, type(ttype)
			{}
			Node node;
			Type type;
		};
		using Chars = std::vector<Char>;
		using Iterator = Chars::iterator;

		void update_space(Iterator i, Node& parent);
		void update_hyphen(Iterator i, Node& parent);
		void update_char(Iterator b, Iterator e, Node& parent);
		void init_string(const Node& node);
		void init_hyphen(const Node& node);
		void init_space(const Node& node);
		void init();

		static Box get_box(const Node& node);
		static void set_box(const Box& box, Node& node);
		static void set_conf(double conf, Node& node);
		static void set_content(const std::wstring& str, Node& node);
		static Iterator find_end_of_token(Iterator b, Iterator e) noexcept;
		static Char make_copy(Char& c);
		static Node merge(Node& a, const Node& b);

		std::vector<Char> chars_;
		Node node_;
		bool needs_update_;
		const bool explicit_spaces_;
	};
}

#endif // pcw_AltoXmlParserLine_hpp__
