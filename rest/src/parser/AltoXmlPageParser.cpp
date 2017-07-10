#include "AltoXmlPageParser.hpp"
#include <cstring>
#include "AltoXmlParserLine.hpp"
#include "XmlParserPage.hpp"
#include "core/Box.hpp"
#include "core/Line.hpp"
#include "core/Page.hpp"
#include "core/util.hpp"
#include "pugixml.hpp"

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
ParserPagePtr AltoXmlPageParser::parse() {
	const auto page = std::make_shared<XmlParserPage>(path_);
	done_ = true;  // alto documents contain just one page
	explicit_spaces_ = has_sp_tags(*page);
	const auto filename =
	    page->xml()
		.doc()
		.select_node(
		    "/alto/Description/sourceImageInformation/fileName")
		.node()
		.child_value();
	page->ocr = path_;
	page->img = fix_windows_path(filename);
	page->file_type = FileType::AltoXml;
	auto p = page->xml().doc().select_node(".//Page");
	parse(p.node(), *page);
	return page;
}

////////////////////////////////////////////////////////////////////////////////
void AltoXmlPageParser::parse(const Xml::Node& pagenode,
			      XmlParserPage& page) const {
	const auto textlines = pagenode.select_nodes(".//TextLine");
	page.box = get_box(pagenode);
	page.id = pagenode.attribute("PHYSICAL_IMG_NR").as_int();
	for (const auto& l : textlines) {
		add_line(l.node(), page);
	}
}

////////////////////////////////////////////////////////////////////////////////
void AltoXmlPageParser::add_line(const Xml::Node& linenode,
				 XmlParserPage& page) const {
	page.lines().push_back(
	    std::make_shared<AltoXmlParserLine>(linenode, explicit_spaces_));
}

////////////////////////////////////////////////////////////////////////////////
Box AltoXmlPageParser::get_box(const Xml::Node& node) {
	const auto l = node.attribute("HPOS").as_int();
	const auto t = node.attribute("VPOS").as_int();
	const auto w = node.attribute("WIDTH").as_int();
	const auto h = node.attribute("HEIGHT").as_int();
	return Box{l, t, l + w, t + h};
}

////////////////////////////////////////////////////////////////////////////////
bool AltoXmlPageParser::has_sp_tags(const XmlParserPage& page) noexcept {
	const auto sp_nodes = page.xml().doc().select_nodes(".//SP");
	return sp_nodes.size();
}
