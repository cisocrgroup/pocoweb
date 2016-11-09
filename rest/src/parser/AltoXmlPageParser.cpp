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
	return do_parse();
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
PagePtr
AltoXmlPageParser::do_parse() const
{
	const auto filename = xml_.select_node(	
		"/alto/Description/sourceImageInformation/fileName"
	).node().child_value();
	Path img = fix_windows_path(filename);
	auto p = xml_.select_node(".//Page");
	auto page = parse(p.node());
	page->img = img;
	page->ocr = path_;
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
PagePtr
AltoXmlPageParser::parse(const XmlNode& pagenode) 
{
	const auto textlines = pagenode.select_nodes(".//TextLine");
	const auto box = get_box(pagenode);
	const auto id = pagenode.attribute("PHYSICAL_IMG_NR").as_int();
	auto page = std::make_shared<Page>(id, box);
	for (const auto& l: textlines) {
		add_line(*page, l.node());
	}
	return page;
}

////////////////////////////////////////////////////////////////////////////////
void
AltoXmlPageParser::add_line(Page& page, const XmlNode& linenode)
{
	const auto id = static_cast<int>(page.size() + 1);
	const auto box = get_box(linenode);
	Line line(id, box);	

	for (const auto& node: linenode.children()) {
		if (strcmp(node.name(), "String") == 0) {
			auto wc = node.attribute("WC").as_double();
			auto box = get_box(node);
			auto token = node.attribute("CONTENT").value();
			line.append(token, box.left(), box.right(), wc);
		} else if (strcmp(node.name(), "SP") == 0) {
			auto wc = node.attribute("WC").as_double();
			auto box = get_box(node);
			line.append(' ', box.right(), wc);
		}
	}
	page.push_back(std::move(line));
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

