#ifndef pcw_docxml_hpp__
#define pcw_docxml_hpp__

#include <pugixml.hpp>

namespace pcw {
	struct DocXml {
		explicit DocXml(pugi::xml_document& d)
			: doc(d) {}
		pugi::xml_document& doc;
	};
	struct DocXmlNode {
		explicit DocXmlNode(const pugi::xml_node& n)
			: node(n) {}
		pugi::xml_node node;
	};

	class BookView;
	class Page;

	DocXml& operator<<(DocXml& j, const BookView& project);
	DocXml& operator<<(DocXml& j, const Page& page);
}

#endif // pcw_docxml_hpp__
