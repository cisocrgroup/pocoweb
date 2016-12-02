#ifndef pcw_XmlFile_hpp__
#define pcw_XmlFile_hpp__

#include <boost/filesystem/path.hpp>
#include "pugixml/pugixml.hpp"

namespace pcw {
	using Path = boost::filesystem::path;
	using XmlDocument = pugi::xml_document;
	using XmlNode = pugi::xml_node;

	class XmlFile {
	public:
		XmlFile();
		XmlFile(const Path& path);
		XmlFile(const std::string& content);
		virtual ~XmlFile() noexcept = default;

		void read(const Path& path);
		void read(const std::string& content);

	protected:
		XmlDocument xml_;
		Path path_;
	};
}

#endif // pcw_XmlFile_hpp__
