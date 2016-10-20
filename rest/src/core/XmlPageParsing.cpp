#include <cstring>
#include <pugixml.hpp>
#include "Book.hpp"
#include "Box.hpp"
#include "Page.hpp"
#include "XmlPageParsing.hpp"

using namespace pugi;
using namespace pcw;

// ALTO
static size_t alto_add_pages(Book& book, const xml_document& xml);
static void alto_add_page(Book& book, const xml_node& page_node);
static void alto_add_lines(Page& page, const xml_node& text_line);
static Box alto_get_box(const xml_node& node);

////////////////////////////////////////////////////////////////////////////////
size_t
pcw::add_pages(Book& book, std::istream& is)
{
	xml_document xml;
	auto ok = xml.load(is);
	if (not ok) 
		throw std::runtime_error("Xml error: " + std::string(ok.description()));
	if (strcmp(xml.document_element().name(), "alto") == 0)
		return alto_add_pages(book, xml);
	return 0;
}

////////////////////////////////////////////////////////////////////////////////
size_t 
alto_add_pages(Book& book, const xml_document& xml)
{
	auto pages = xml.select_nodes(".//Page");
	size_t i = 0;
	for (const auto& n: pages) {
		alto_add_page(book, n.node());
		++i;
	}
	return i;
}

////////////////////////////////////////////////////////////////////////////////
void 
alto_add_page(Book& book, const xml_node& page_node)
{

	auto text_lines = page_node.select_nodes(".//TextLine");
	auto box = alto_get_box(page_node);
	auto id = static_cast<int>(book.size() + 1);
	auto page = std::make_shared<Page>(book, id, box);
	book.push_back(page);
	for (const auto& n: text_lines) {
		alto_add_lines(*page, n.node());
	}
}

////////////////////////////////////////////////////////////////////////////////
void 
alto_add_lines(Page& page, const xml_node& text_line)
{
	const auto id = static_cast<int>(page.size() + 1);
	const auto box = alto_get_box(text_line);
	Line line(page, id, box);

	for (const auto& node: text_line.children()) {
		if (strcmp(node.name(), "String") == 0) {
			const auto box = alto_get_box(node);
			line.append(node.attribute("CONTENT").value(), box);
		} else if (strcmp(node.name(), "SP") == 0) {
			const auto box = alto_get_box(node);
			line.append(' ', box);
		}
	}
	page.push_back(std::move(line));
}

////////////////////////////////////////////////////////////////////////////////
Box 
alto_get_box(const xml_node& node)
{
	const auto l = node.attribute("VPOS").as_int();
	const auto t = node.attribute("HPOS").as_int();
	const auto w = node.attribute("WIDTH").as_int();
	const auto h = node.attribute("HEIGHT").as_int();
	return Box{l, t, l + w, t + h};
}
