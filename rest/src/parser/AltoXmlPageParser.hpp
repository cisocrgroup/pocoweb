#ifndef pcw_AltoXmlPageParser_hpp__
#define pcw_AltoXmlPageParser_hpp__

#include <boost/filesystem/path.hpp>
#include <memory>
#include "core/Box.hpp"
#include "core/PageParser.hpp"
#include "core/XmlFile.hpp"

namespace pcw {
	class XmlParserPage;
	using XmlParserPagePtr = std::shared_ptr<XmlParserPage>;

	class AltoXmlPageParser: public PageParser,
				 public XmlFile {
	public:
		AltoXmlPageParser(const Path& path);
		virtual ~AltoXmlPageParser() noexcept override = default;
		virtual bool has_next() const noexcept override {return not done_;}
		virtual PagePtr parse() override;

		XmlParserPagePtr pparse();

	private:
		static void parse(const XmlNode& pagenode, XmlParserPage& page);
		static void add_line(const XmlNode& linenode, XmlParserPage& page);
		static Box get_box(const XmlNode& node);

		bool done_;
	};
}

#endif // pcw_AltoXmlPageParser_hpp__
