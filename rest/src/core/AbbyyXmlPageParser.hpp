#ifndef pcw_AbbyyXmlPageParser_hpp__
#define pcw_AbbyyXmlPageParser_hpp__

#include <boost/filesystem/path.hpp>
#include <memory>
#include "PageParser.hpp"
#include "XmlFile.hpp"

namespace pcw {
	class Box;

	class AbbyyXmlPageParser: public PageParser, 
				  public XmlFile {
	public:
		static bool is_alto_document(const XmlDocument& xml);
	
		AbbyyXmlPageParser(const Path& path);
		virtual ~AbbyyXmlPageParser() noexcept override = default;
		virtual bool has_next() const noexcept override {return not done_;}
		virtual PagePtr next() override;

	private:
		PagePtr parse() const;

		static PagePtr parse(const XmlNode& pagenode);
		static void add_line(Page& page, const XmlNode& linenode);
		static Box get_box(const XmlNode& node);

		bool done_;
	};
}

#endif // pcw_AbbyyXmlPageParser_hpp__
