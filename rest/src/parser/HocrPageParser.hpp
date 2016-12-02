#ifndef pcw_HocrPageParser_hpp__
#define pcw_HocrPageParser_hpp__

#include <boost/filesystem/path.hpp>
#include <memory>
#include "pugixml/pugixml.hpp"
#include "PageParser.hpp"
#include "Xml.hpp"

namespace pcw {
	class Box;
	class ParserPage;
	class XmlParserPage;
	using ParserPagePtr = std::shared_ptr<ParserPage>;
	using XmlParserPagePtr = std::shared_ptr<XmlParserPage>;

	class HocrPageParser: public PageParser,
			      public pugi::xml_tree_walker {
	public:
		HocrPageParser(Xml xml);
		HocrPageParser(const Path& path);
		virtual ~HocrPageParser() noexcept override = default;
		virtual bool has_next() const noexcept override {
			return page_node_;
		}
		virtual ParserPagePtr parse() override;
		virtual bool begin(Xml::Node& node) override;
		virtual bool for_each(Xml::Node& node) override;

	private:
		void next_page();

		Xml xml_;
		Path path_;
		Xml::Node page_node_;
		XmlParserPagePtr page_;
	};
}

#endif // pcw_HocrPageParser_hpp__
