#include "PageXmlPageParser.hpp"
#include "XmlParserPage.hpp"
#include "core/Box.hpp"
#include "core/Line.hpp"
#include "core/Page.hpp"
#include "core/util.hpp"
#include "pugixml.hpp"
#include <cstring>

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
ParserPagePtr
PageXmlPageParser::parse()
{
  const auto page = std::make_shared<XmlParserPage>(path_);
  done_ = true; // page documents contain just one page
  const auto pagenode =
    page->xml().doc().select_node("/pc:PcGts/pc:Page").node();
  const auto filename = pagenode.attribute("imageFilename").value();
  page->ocr = path_;
  page->img = fix_windows_path(filename);
  page->file_type = FileType::PageXml;
  // auto p = page->xml().doc().select_node(".//Page");
  parse(pagenode, *page);
  return page;
}

////////////////////////////////////////////////////////////////////////////////
void
PageXmlPageParser::parse(const Xml::Node& pagenode, XmlParserPage& page) const
{
  //  const auto textlines = pagenode.select_nodes(".//TextLine");
  page.box = get_box(pagenode);
  // page.id = pagenode.attribute("PHYSICAL_IMG_NR").as_int();
  // for (const auto& l : textlines) {
  //   add_line(l.node(), page);
  // }
}

////////////////////////////////////////////////////////////////////////////////
void
PageXmlPageParser::add_line(const Xml::Node& linenode,
                            XmlParserPage& page) const
{
  // page.lines().push_back(
  //   std::make_shared<PageXmlParserLine>(linenode, explicit_spaces_));
}

////////////////////////////////////////////////////////////////////////////////
Box
PageXmlPageParser::get_box(const Xml::Node& node)
{
  const auto r = node.attribute("imageWidth").as_int();
  const auto b = node.attribute("imageHeight").as_int();
  return Box{ 0, 0, r, b };
}
