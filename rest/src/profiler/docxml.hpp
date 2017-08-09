#ifndef pcw_docxml_hpp__
#define pcw_docxml_hpp__

#include <vector>
#include "Profile.hpp"
#include "pugixml.hpp"

namespace pcw {
class Project;
class Page;
class Profile;
struct Suggestion;

struct DocXml : public pugi::xml_document {
	DocXml() : suggestions(nullptr) {}
	const Profile::Suggestions* suggestions;
};

DocXml& operator<<(DocXml& docxml, const Project& view);
DocXml& operator<<(DocXml& docxml, const Page& page);
DocXml& operator<<(DocXml& docxml, const Profile& profile);
}

#endif  // pcw_docxml_hpp__
