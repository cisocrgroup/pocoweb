#ifndef pcw_docxml_hpp__
#define pcw_docxml_hpp__

#include <vector>
#include <pugixml.hpp>

namespace pcw {
	class Profile;
	class Suggestion;

	struct DocXml: public pugi::xml_document {
		DocXml(): suggestions_(nullptr) {}
		const std::vector<Suggestion>* suggestions_;
	};

	class BookView;
	class Page;

	DocXml& operator<<(DocXml& docxml, const BookView& view);
	DocXml& operator<<(DocXml& docxml, const Page& page);
	DocXml& operator<< (DocXml& docxml, const Profile& profile);
}

#endif // pcw_docxml_hpp__
