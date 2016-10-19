#include <cstring>
#include <pugixml.hpp>
#include "Book.hpp"
#include "Page.hpp"
#include "XmlPageParsing.hpp"

using namespace pugi;
using namespace pcw;

// ALTO
static size_t alto_add_pages(Book& book, const xml_document& xml);
static void alto_add_page(Book& book, const xml_node& page_node);
static void alto_add_lines(Page& page, const xml_node& text_line);

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
	auto page = std::make_shared<Page>(book, static_cast<int>(book.size() + 1));
	book.push_back(page);
	for (const auto& n: text_lines) {
		alto_add_lines(*page, n.node());
	}
}

////////////////////////////////////////////////////////////////////////////////
void 
alto_add_lines(Page& page, const xml_node& text_line)
{
	auto strings = text_line.select_nodes(".//String");
	std::string line;
	for (const auto& n: strings) {
		line.append(n.node().attribute("CONTENT").value());
		line.append(" ");
	}
	if (not line.empty())
		line.pop_back(); // discard last ' '
	page.push_back(std::move(line));
}

// static const std::string string{"String"};
//         static const std::string content{"CONTENT"};
//         static const std::string space{"SP"};
//         if (strcmp(node.name(), "String") == 0) {
//                 auto content = node.attribute("CONTENT");
//                 if (content)
//                         buffer().append(content.value());
//         } else if (strcmp(node.name(), "TextLine") == 0 or
//                    strcmp(node.name(), "SP") == 0) {
//                 buffer().push_back(' ');
//         }
//         return true;
// 
