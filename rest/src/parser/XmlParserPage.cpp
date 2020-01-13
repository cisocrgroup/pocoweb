#include "XmlParserPage.hpp"
#include "pugixml.hpp"
#include <boost/filesystem/operations.hpp>
#include <crow/logging.h>
#include <stdexcept>

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
void XmlParserPage::write(const Path &path) const {
  CROW_LOG_DEBUG << "(XmlParserPage) writing xml to " << path;
  boost::filesystem::create_directories(path.parent_path());
  xml_.write(path);
}
