#include <stdexcept>
#include <pugixml.hpp>
#include "XmlParserPage.hpp"

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
void
XmlParserPage::write(const Path& path) const
{
	xml_.write(path);
}
