#ifndef pcw_AltoXmlPageParser_hpp__
#define pcw_AltoXmlPageParser_hpp__

#include <boost/filesystem/path.hpp>
#include <memory>
#include "core/Box.hpp"
#include "core/PageParser.hpp"
#include "core/XmlFile.hpp"

namespace pcw {
	class ParserPage;

	class AltoXmlPageParser: public PageParser,
				 public XmlFile {
	public:
		AltoXmlPageParser(const Path& path);
		virtual ~AltoXmlPageParser() noexcept override = default;
		virtual bool has_next() const noexcept override {return not done_;}
		virtual PagePtr parse() override;

		ParserPage pparse();

	private:
		PagePtr do_parse() const;

		static PagePtr parse(const XmlNode& pagenode);
		static void parse(const XmlNode& pagenode, ParserPage& page);
		static void add_line(Page& page, const XmlNode& linenode);
		static void add_line(const XmlNode& linenode, ParserPage& page);
		static Box get_box(const XmlNode& node);

		bool done_;
	};
}

#endif // pcw_AltoXmlPageParser_hpp__
