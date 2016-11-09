#include <cstring>
#include <pugixml.hpp>
#include "AltoXmlPageParser.hpp"
#include "AltoXmlParserLine.hpp"
#include "XmlParserPage.hpp"
#include "core/Page.hpp"
#include "core/Line.hpp"
#include "core/Box.hpp"
#include "core/util.hpp"

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
AltoXmlPageParser::AltoXmlPageParser(const Path& path)
	: XmlFile(path)
	, done_(false)
{
}

////////////////////////////////////////////////////////////////////////////////
PagePtr 
AltoXmlPageParser::parse() 
{
	done_ = true; // alto documents contain just one page
	return pparse()->page();
}

////////////////////////////////////////////////////////////////////////////////
XmlParserPagePtr
AltoXmlPageParser::pparse() 
{
	done_ = true; // alto documents contain just one page
	auto page = std::make_shared<XmlParserPage>(path_);
	const auto filename = page->doc().select_node(	
		"/alto/Description/sourceImageInformation/fileName"
	).node().child_value();
	page->ocr = path_;
	page->img = fix_windows_path(filename);
	auto p = page->doc().select_node(".//Page");
	parse(p.node(), *page);
	return page;
}

////////////////////////////////////////////////////////////////////////////////
void
AltoXmlPageParser::parse(const XmlNode& pagenode, XmlParserPage& page) 
{
	const auto textlines = pagenode.select_nodes(".//TextLine");
	page.box = get_box(pagenode);
	page.id = pagenode.attribute("PHYSICAL_IMG_NR").as_int();
	for (const auto& l: textlines) {
		add_line(l.node(), page);
	}
}

////////////////////////////////////////////////////////////////////////////////
void
AltoXmlPageParser::add_line(const XmlNode& linenode, XmlParserPage& page)
{
	page.lines().push_back(std::make_shared<AltoXmlParserLine>(linenode));
}

////////////////////////////////////////////////////////////////////////////////
Box 
AltoXmlPageParser::get_box(const XmlNode& node)
{
	const auto l = node.attribute("HPOS").as_int();
	const auto t = node.attribute("VPOS").as_int();
	const auto w = node.attribute("WIDTH").as_int();
	const auto h = node.attribute("HEIGHT").as_int();
	return Box{l, t, l + w, t + h};
}

