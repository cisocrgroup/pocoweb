#include <cstring>
#include <iostream>
#include <pugixml.hpp>
#include <regex>
#include "core/BadRequest.hpp"
#include "HocrPageParser.hpp"
#include "core/Page.hpp"
#include "core/Line.hpp"
#include "core/Box.hpp"
#include "core/util.hpp"

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
HocrPageParser::HocrPageParser(const Path& path)
	: XmlFile(path)
	, page_node_()
	, page_()
{
	page_node_ = xml_.select_node("/html/body/div[@class='ocr_page']").node();
}

////////////////////////////////////////////////////////////////////////////////
PagePtr 
HocrPageParser::parse() 
{
	assert(page_node_);
	page_node_.traverse(*this);
	assert(page_);
	next_page();
	return page_;
}

////////////////////////////////////////////////////////////////////////////////
bool
HocrPageParser::begin(XmlNode& node)
{
	auto box = get_box(node);
	auto img = get_img(node);
	page_ = std::make_shared<Page>(0, box);
	page_->ocr = path_;
	page_->img = img;
	return true;
}

////////////////////////////////////////////////////////////////////////////////
bool
HocrPageParser::for_each(XmlNode& node)
{
	if (strcasecmp(node.name(), "span") == 0) {
		if (strcmp(node.attribute("class").value(), "ocr_line") == 0) {
			assert(page_);
			auto box = get_box(node);
			auto id = static_cast<int>(page_->size() + 1);
			page_->push_back({id, box});
		} else if (strcmp(node.attribute("class").value(), "ocrx_word") == 0) {
			if (page_->empty())
				throw std::runtime_error("(HocrPageParser) Missing class='ocr_line'");
			auto box = get_box(node);
			auto conf = get_conf(node);
			if (not page_->back().empty()) {
				page_->back().append(' ', box.left(), 0);
			}
			page_->back().append(node.child_value(), box.left(), box.right(), conf);
		}
	}
	return true;
}

////////////////////////////////////////////////////////////////////////////////
void
HocrPageParser::next_page()
{
	bool done = false;
	while (page_node_ and not done) {
		page_node_ = page_node_.next_sibling("div");
		if (strcmp(page_node_.attribute("class").value(), "ocr_page") == 0)
			done = true;
	}
}

////////////////////////////////////////////////////////////////////////////////
double
HocrPageParser::get_conf(const XmlNode& node)
{
	static const std::regex re{R"(x_wconf\s+(\d+))"};
	std::cmatch m;
	if (std::regex_search(node.attribute("title").value(), m, re))
		return static_cast<double>(std::stoi(m[1])) / 100;
	return 0;
}

////////////////////////////////////////////////////////////////////////////////
std::string
HocrPageParser::get_img(const XmlNode& node)
{
	static const std::regex re{R"xx(title\s+"(.*)")xx"};
	std::cmatch m;
	if (std::regex_search(node.attribute("title").value(), m, re))
		return m[1];
	return {};
}

////////////////////////////////////////////////////////////////////////////////
Box
HocrPageParser::get_box(const XmlNode& node)
{
	static const std::regex re{R"(bbox\s+(\d+)\s+(\d+)\s+(\d+)\s+(\d+))"};
	std::cmatch m;
	if (not std::regex_search(node.attribute("title").value(), m, re))
		throw BadRequest("(HocrPageParser) Missing bbox");
	return Box {std::stoi(m[1]), std::stoi(m[2]), std::stoi(m[3]), std::stoi(m[4])};
}

