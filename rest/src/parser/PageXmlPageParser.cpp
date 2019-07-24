#include "PageXmlPageParser.hpp"
#include "PageXmlParserLine.hpp"
#include "XmlParserPage.hpp"
#include "core/Box.hpp"
#include "core/Line.hpp"
#include "core/Page.hpp"
#include "core/util.hpp"
#include "pugixml.hpp"
#include <cstring>

using namespace pcw;

template <class F> void each_text_region(const Xml::Node &pagenode, F f);

////////////////////////////////////////////////////////////////////////////////
ParserPagePtr PageXmlPageParser::parse() {
  const auto page = std::make_shared<XmlParserPage>(path_);
  const auto pagenode =
      page->xml()
          .doc()
          .select_node("/*[local-name()='PcGts']/*[local-name()='Page']")
          .node();
  page->ocr = path_;
  parse(pagenode, *page);
  done_ = true; // page documents contain just one page
  return page;
}

////////////////////////////////////////////////////////////////////////////////
void PageXmlPageParser::parse(const Xml::Node &pagenode,
                              XmlParserPage &page) const {
  const auto filename = pagenode.attribute("imageFilename").value();
  page.img = fix_windows_path(filename);
  page.file_type = FileType::PageXml;
  page.box = get_page_box(pagenode);
  each_text_region(pagenode, [&](const auto &region_node) {
    for (const auto &line :
         region_node.select_nodes("./*[local-name()='TextLine']")) {
      add_line(line.node(), page);
    }
  });
  // page.id = pagenode.attribute("PHYSICAL_IMG_NR").as_int();
  // for (const auto& l : textlines) {
  //   add_line(l.node(), page);
  // }
}

////////////////////////////////////////////////////////////////////////////////
void PageXmlPageParser::add_line(const Xml::Node &linenode,
                                 XmlParserPage &page) const {
  page.lines().push_back(
      std::make_shared<PageXmlParserLine>(linenode, page.lines().empty()));
}

////////////////////////////////////////////////////////////////////////////////
Box PageXmlPageParser::get_page_box(const Xml::Node &node) {
  const auto r = node.attribute("imageWidth").as_int();
  const auto b = node.attribute("imageHeight").as_int();
  return Box{0, 0, r, b};
}

static std::vector<std::string>
get_ordered_text_regions(const Xml::Node &pagenode) {
  std::vector<std::pair<int, std::string>> text_regions;
  for (const auto &region_ref :
       pagenode.select_nodes("./ReadingOrder/OrderedGroup/RegionRefIndexed")) {
    text_regions.emplace_back(region_ref.node().attribute("index").as_int(),
                              region_ref.node().attribute("regionRef").value());
  }
  // sort by index
  std::sort(text_regions.begin(), text_regions.end(),
            [](const auto &a, const auto &b) { return a.first < b.first; });
  // transform to orderd list of region refs
  std::vector<std::string> ordered_text_regions;
  std::transform(text_regions.begin(), text_regions.end(),
                 std::back_inserter(ordered_text_regions),
                 [](const auto &p) -> std::string { return p.second; });
  return ordered_text_regions;
}

////////////////////////////////////////////////////////////////////////////////
template <class F> void each_text_region(const Xml::Node &pagenode, F f) {
  const auto text_regions = get_ordered_text_regions(pagenode);
  if (not text_regions.empty()) {
    for (const auto &region_id : text_regions) {
      const auto region_ref = "./TextRegion[@id='" + region_id + "']";
      for (const auto &region : pagenode.select_nodes(region_ref.data())) {
        f(region.node());
      }
    }
  } else {
    for (const auto &region : pagenode.select_nodes("./TextRegion")) {
      f(region.node());
    }
  }
}
