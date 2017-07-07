#include "XmlParserPage.hpp"
#include <crow/logging.h>
#include <boost/filesystem/operations.hpp>
#include <stdexcept>
#include "pugixml.hpp"

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
void XmlParserPage::write(const Path& path) const {
	CROW_LOG_DEBUG << "(XmlParserPage) writing xml to " << path;
	boost::filesystem::create_directories(path.parent_path());
	xml_.write(path);
}
