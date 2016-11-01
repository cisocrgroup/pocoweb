#ifndef pcw_AltoXmlPageParser_hpp__
#define pcw_AltoXmlPageParser_hpp__

#include <boost/filesystem/path.hpp>
#include <memory>
#include "PageParser.hpp"
#include "XmlFile.hpp"

namespace pcw {
	class Box;

	class AltoXmlPageParser: public PageParser,
				 public XmlFile {
	public:
		AltoXmlPageParser(const Path& path);
		virtual ~AltoXmlPageParser() noexcept override = default;
		virtual bool has_next() const noexcept override {return not done_;}
		virtual PagePtr parse() override;

	private:
		PagePtr parse() const;

		static PagePtr parse(const XmlNode& pagenode);
		static void add_line(Page& page, const XmlNode& linenode);
		static Box get_box(const XmlNode& node);

		bool done_;
	};
}

#endif // pcw_AltoXmlPageParser_hpp__
