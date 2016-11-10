#ifndef pcw_AltoXmlPageParser_hpp__
#define pcw_AltoXmlPageParser_hpp__

#include <boost/filesystem/path.hpp>
#include <memory>
#include "core/PageParser.hpp"
#include "Xml.hpp"

namespace pcw {
	class Box;
	class XmlParserPage;
	using XmlParserPagePtr = std::shared_ptr<XmlParserPage>;

	class AltoXmlPageParser: public PageParser {
	public:
		AltoXmlPageParser(const Path& path): path_(path), xml_(path), done_(false) {}
		AltoXmlPageParser(Xml xml): xml_(xml), done_(false) {}
		virtual ~AltoXmlPageParser() noexcept override = default;
		virtual bool has_next() const noexcept override {return not done_;}
		virtual PagePtr parse() override;

		XmlParserPagePtr pparse();

	private:
		static void parse(const Xml::Node& pagenode, XmlParserPage& page);
		static void add_line(const Xml::Node& linenode, XmlParserPage& page);
		static Box get_box(const Xml::Node& node);

		Path path_;
		Xml xml_;
		bool done_;
	};
}

#endif // pcw_AltoXmlPageParser_hpp__
