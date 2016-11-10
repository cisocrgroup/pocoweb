#ifndef pcw_AbbyyXmlPageParser_hpp__
#define pcw_AbbyyXmlPageParser_hpp__

#include <boost/filesystem/path.hpp>
#include <memory>
#include <pugixml.hpp>
#include "core/PageParser.hpp"
#include "Xml.hpp"

namespace pcw {
	class Box;

	class AbbyyXmlPageParser: public PageParser {
	public:
		static bool is_alto_document(const Xml::Doc& xml);

		AbbyyXmlPageParser(Xml xml);
		AbbyyXmlPageParser(const Path& path);
		virtual ~AbbyyXmlPageParser() noexcept override = default;
		virtual bool has_next() const noexcept override;
		virtual PagePtr parse() override;

	private:
		Path path_;
		Xml xml_;
		Xml::Node page_;
	};
}

#endif // pcw_AbbyyXmlPageParser_hpp__
