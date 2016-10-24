#include <cstring>
#include <pugixml.hpp>
#include "AltoXmlPageParser.hpp"
#include "Page.hpp"
#include "Line.hpp"
#include "Box.hpp"
#include "util.hpp"

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
bool 
AltoXmlPageParser::is_alto_document(const XmlDocument& xml)
{
	return strcmp(xml.document_element().name(), "alto") == 0;
}

////////////////////////////////////////////////////////////////////////////////
AltoXmlPageParser::AltoXmlPageParser(XmlDocumentPtr xml, Path path)
	: path_(std::move(path))
	, xml_(std::move(xml))
	, done_(false)
{
	assert(xml_);
}

////////////////////////////////////////////////////////////////////////////////
PagePtr 
AltoXmlPageParser::next() 
{
	done_ = true; // alto documents contain just one page
	return parse();
}

////////////////////////////////////////////////////////////////////////////////
PagePtr
AltoXmlPageParser::parse() const
{
	const auto filename = xml_->select_node(	
		"/alto/Description/sourceImageInformation/fileName"
	).node().child_value();
	Path img = fix_windows_path(filename);
	auto p = xml_->select_node(".//Page");
	auto page = parse(p.node());
	page->img = img;
	page->ocr = path_;
	return page;
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
			line.append(' ', box.left(), box.right(), wc);
		}
	}
	page.push_back(std::move(line));
}

////////////////////////////////////////////////////////////////////////////////
Box 
AltoXmlPageParser::get_box(const XmlNode& node)
{
	const auto l = node.attribute("VPOS").as_int();
	const auto t = node.attribute("HPOS").as_int();
	const auto w = node.attribute("WIDTH").as_int();
	const auto h = node.attribute("HEIGHT").as_int();
	return Box{l, t, l + w, t + h};
}

