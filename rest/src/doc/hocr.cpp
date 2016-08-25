#include <iostream>
#include <regex>
#include <memory>
#include <vector>
#include <boost/log/trivial.hpp>
#include <boost/filesystem.hpp>
#include <pugixml.hpp>
#include <json.hpp>
#include "doc/Container.hpp"
#include "doc/BookData.hpp"
#include "doc/Box.hpp"
#include "doc/Line.hpp"
#include "doc/Page.hpp"
#include "doc/Book.hpp"
#include "hocr.hpp"

namespace fs = boost::filesystem;

///////////////////////////////////////////////////////////////////////////////
static bool
is_hocr_file(const fs::path& p)
{
	return fs::is_regular_file(p) and (
		p.extension().string() == ".xml" or
		p.extension().string() == ".hocr" or
		p.extension().string() == ".html" or
		p.extension().string() == ".htm");
}

// ///////////////////////////////////////////////////////////////////////////////
// static std::string
// parse_image(pugi::xml_node node)
// {
// 	static const std::regex imagere{R"(image\s+\"?([^\"]*)\"?)"};
// 	std::cmatch m;
// 	if (std::regex_search(node.attribute("title").value(), m, imagere))
// 		return m[1];
// 	throw std::runtime_error(
// 		std::string("HOCR missing bbox in `") +
// 		node.attribute("title").value() + "`");
// }

///////////////////////////////////////////////////////////////////////////////
static pcw::Box
parse_box(pugi::xml_node node)
{
	static const std::regex bboxre{
		R"(bbox\s+(\d+)\s+(\d+)\s+(\d+)\s+(\d+))"
	};
	std::cmatch m;
	if (std::regex_search(node.attribute("title").value(), m, bboxre)) 
		return {
			std::stoi(m[1]),
			std::stoi(m[2]),
			std::stoi(m[3]),
			std::stoi(m[4])
		};
	throw std::runtime_error(
		std::string("HOCR missing bbox in `") + 
		node.attribute("title").value() + "`");
}

///////////////////////////////////////////////////////////////////////////////
static void
append_token(pcw::Line& line, 
	     const pcw::Box& tbox, 
	     const pcw::Box& lbox, 
	     const char *token)
{
	auto offset = tbox.left - lbox.left;
	const auto n = strlen(token);
	const auto w = tbox.right - tbox.left;
	const auto d = n ? w / n : w;
	
	for (int i = 0; i < (int)n; ++i) {
		line.cuts().push_back(offset);
		offset += d;
	}		
	line.cuts().push_back(tbox.right - lbox.left + 1);
	line.line().append(token);
	line.line().append(1, ' ');
}

///////////////////////////////////////////////////////////////////////////////
static pcw::LinePtr
parse_hocr_line(pugi::xml_node node)
{
	auto tokens = node.select_nodes(".//span[@class='ocrx_word']");
	// TODO: FIX id=1
	auto line = std::make_shared<pcw::Line>(1);
	line->box = parse_box(node);
	for (const auto& t: tokens) {
		const char *token = t.node().text().get();
		const auto box = parse_box(t.node());
		append_token(*line, box, line->box, token);
	}
	assert(line->line().size() == line->cuts().size());
	return line;
}

///////////////////////////////////////////////////////////////////////////////
static void
parse_hocr_page(pugi::xml_node node, pcw::Page& page)
{
	auto lines = node.select_nodes(".//span[@class='ocr_line']");
	int linen = 0;
	for (const auto& l: lines) {
		page.push_back(parse_hocr_line(l.node()));
		page.back()->id = ++linen;
	}
	page.box = parse_box(node);
	// page.imageimage= parse_image(node);
}

///////////////////////////////////////////////////////////////////////////////
void
pcw::parse_hocr_page(Page& page)
{
	pugi::xml_document doc;
	auto ok = doc.load_file(page.ocrfile.string().data());
	if (not ok) {
		BOOST_LOG_TRIVIAL(error) << "XML error: " << ok.description();
		throw std::runtime_error(ok.description());
	}
	
	auto page_nodes = doc.select_nodes("//div[@class='ocr_page']");
	int n = 0;
	for (const auto& page_node: page_nodes) {
		++n;
		::parse_hocr_page(page_node.node(), page);
	}
	if (n != 1)
		throw std::runtime_error("Cannot handle multiple pages per file");
}

///////////////////////////////////////////////////////////////////////////////
pcw::PagePtr
pcw::parse_hocr_page(const Path& p)
{
	pugi::xml_document doc;
	auto ok = doc.load_file(p.string().data());
	if (not ok) {
		BOOST_LOG_TRIVIAL(error) << "XML error: " << ok.description();
		return nullptr;
	}
	
	try {
		auto pages = doc.select_nodes("//div[@class='ocr_page']");
		int n = 0;
		PagePtr page = nullptr;
		for (const auto& p: pages) {
			page = std::make_shared<Page>(++n);
			::parse_hocr_page(p.node(), *page);
		}
		if (n != 1)
			return nullptr;
		return page;
	} catch (const std::exception& e) {
		BOOST_LOG_TRIVIAL(error) << "XML error: " << e.what();
		return nullptr;
	}
}

///////////////////////////////////////////////////////////////////////////////
pcw::BookPtr
pcw::parse_hocr_book(const Path& dir)
{
	// TODO: fix id=1 hack
	BookPtr book = std::make_shared<Book>(1);
	int n = 0;
	for (auto i = fs::directory_iterator(dir); i != fs::directory_iterator(); ++i) {
		if (is_hocr_file(*i)) {
			book->push_back(parse_hocr_page(*i));
			if (not book->back())
				return nullptr;
			book->back()->id = ++n;
		}
	}
	return book;
}


