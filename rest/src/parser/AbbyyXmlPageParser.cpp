#include "AbbyyXmlPageParser.hpp"
#include "AbbyyXmlParserLine.hpp"
#include "XmlParserPage.hpp"
#include "core/Box.hpp"
#include "core/util.hpp"
#include "pugixml.hpp"
#include <cstring>

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
AbbyyXmlPageParser::AbbyyXmlPageParser(Xml xml) : path_(), xml_(xml), page_() {
  page_ = xml_.doc().select_node("/document/page").node();
}

////////////////////////////////////////////////////////////////////////////////
AbbyyXmlPageParser::AbbyyXmlPageParser(const Path &path)
  : path_(path), xml_(path), page_() {
  page_ = xml_.doc().select_node("/document/page").node();
}

////////////////////////////////////////////////////////////////////////////////
ParserPagePtr AbbyyXmlPageParser::parse() {
  assert(page_);
  auto page = std::make_shared<XmlParserPage>(xml_);
  page->ocr = path_;
  if (page->id == 0) {
    page->id = guess_id(path_);
  }
  page->box = {0, 0, page_.attribute("width").as_int(),
               page_.attribute("height").as_int()};
  page->file_type = FileType::AbbyyXml;
  auto linenodes = page_.select_nodes(".//line");
  for (const auto &i : linenodes) {
    page->lines().push_back(std::make_shared<AbbyyXmlParserLine>(i.node()));
  }
  page_ = page_.next_sibling("page");
  return page;
}
