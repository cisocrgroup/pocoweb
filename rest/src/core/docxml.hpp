#ifndef pcw_docxml_hpp__
#define pcw_docxml_hpp__

#include <vector>
#include <pugixml.hpp>

namespace pcw {
	class BookView;
	class Page;
	class Profile;
	class Suggestion;

	struct DocXml: public pugi::xml_document {
		DocXml(): suggestions(nullptr) {}
		const std::vector<Suggestion>* suggestions;
	};

	DocXml& operator<<(DocXml& docxml, const BookView& view);
	DocXml& operator<<(DocXml& docxml, const Page& page);
	DocXml& operator<< (DocXml& docxml, const Profile& profile);
}

#endif // pcw_docxml_hpp__
