#include <cstring>
#include <fstream>
#include <pugixml.hpp>
#include "Book.hpp"
#include "Box.hpp"
#include "Page.hpp"
#include "XmlPageParsing.hpp"

namespace fs = boost::filesystem;
using namespace pugi;
using namespace pcw;

static size_t add_pages(const fs::path& p, Book& book, std::istream& is);

// ALTO
static size_t alto_add_pages(const fs::path& p, Book& book, const xml_document& xml);
static void alto_add_page(Book& book, const xml_node& page_node);
static void alto_add_lines(Page& page, const xml_node& text_line);
static Box alto_get_box(const xml_node& node);
static double alto_get_confidence(const xml_node& node);


////////////////////////////////////////////////////////////////////////////////
size_t
pcw::add_pages(const fs::path& path, Book& book)
{
	std::ifstream is(path.string());
	if (not is.good())
		throw std::system_error(errno, std::system_category(), path.string());
	return ::add_pages(path, book, is);
}

////////////////////////////////////////////////////////////////////////////////
size_t
add_pages(const fs::path& p, Book& book, std::istream& is)
{
	xml_document xml;
	auto ok = xml.load(is);
	if (not ok) 
		throw std::runtime_error("Xml error: " + std::string(ok.description()));
	if (strcmp(xml.document_element().name(), "alto") == 0) 
		return alto_add_pages(p, book, xml);
	return 0;
}

////////////////////////////////////////////////////////////////////////////////
size_t 
alto_add_pages(const fs::path& p, Book& book, const xml_document& xml)
{
	std::string tmp = xml.select_node(
		"/alto/Description/sourceImageInformation/fileName"
	).node().child_value();
	std::replace(begin(tmp), end(tmp), '\\', '/'); // fix windows paths?
	fs::path img(tmp);
	auto pages = xml.select_nodes(".//Page");
	size_t i = 0;
	for (const auto& n: pages) {
		alto_add_page(book, n.node());
		if (not book.empty()) {
			assert(book.back());
			book.back()->img = img;
			book.back()->ocr = p;
			++i;
		}
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
			const auto conf = alto_get_confidence(node);
			const auto box = alto_get_box(node);
			line.append(node.attribute("CONTENT").value(), box.left(), box.right(), conf);
		} else if (strcmp(node.name(), "SP") == 0) {
			const auto conf = alto_get_confidence(node);
			const auto box = alto_get_box(node);
			line.append(' ', box.left(), box.right(), conf);
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

////////////////////////////////////////////////////////////////////////////////
double 
alto_get_confidence(const xml_node& node)
{
	return node.attribute("WC").as_double();
}

