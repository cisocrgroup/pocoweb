#include <cstring>
#include <iostream>
#include <pugixml.hpp>
#include <regex>
#include "core/BadRequest.hpp"
#include "HocrPageParser.hpp"
#include "HocrParserLine.hpp"
#include "ParserPage.hpp"
#include "XmlParserPage.hpp"
#include "core/util.hpp"

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
HocrPageParser::HocrPageParser(Xml xml)
	: xml_(std::move(xml))
	, path_()
	, page_node_()
	, page_()
{
	page_node_ = xml_.doc().select_node(
		"/html/body/div[@class='ocr_page']"
	).node();
}

////////////////////////////////////////////////////////////////////////////////
HocrPageParser::HocrPageParser(const Path& path)
	: xml_(path)
	, path_(path)
	, page_node_()
	, page_()
{
	page_node_ = xml_.doc().select_node(
		"/html/body/div[@class='ocr_page']"
	).node();
}

////////////////////////////////////////////////////////////////////////////////
PagePtr 
HocrPageParser::parse() 
{
	return pparse()->page();
}

////////////////////////////////////////////////////////////////////////////////
ParserPagePtr 
HocrPageParser::pparse() 
{
	assert(page_node_);
	page_node_.traverse(*this);
	assert(page_);
	next_page();
	return page_;
}

////////////////////////////////////////////////////////////////////////////////
bool
HocrPageParser::begin(Xml::Node& node)
{
	page_ = std::make_shared<XmlParserPage>(xml_);
	page_->ocr = path_;
	page_->img = get_img(node);
	page_->box = get_box(node);
	return true;
}

////////////////////////////////////////////////////////////////////////////////
bool
HocrPageParser::for_each(Xml::Node& node)
{
	if (strcasecmp(node.name(), "span") == 0) {
		if (strcmp(node.attribute("class").value(), "ocr_line") == 0) {
			assert(page_);
			page_->lines().push_back(
				std::make_shared<HocrParserLine>(node)
			);
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
HocrPageParser::get_conf(const Xml::Node& node)
{
	static const std::regex re{R"(x_wconf\s+(\d+))"};
	std::cmatch m;
	if (std::regex_search(node.attribute("title").value(), m, re))
		return static_cast<double>(std::stoi(m[1])) / 100;
	return 0;
}

////////////////////////////////////////////////////////////////////////////////
std::string
HocrPageParser::get_img(const Xml::Node& node)
{
	static const std::regex re{R"xx(title\s+"(.*)")xx"};
	std::cmatch m;
	if (std::regex_search(node.attribute("title").value(), m, re))
		return m[1];
	return {};
}

////////////////////////////////////////////////////////////////////////////////
Box
HocrPageParser::get_box(const Xml::Node& node)
{
	static const std::regex re{R"(bbox\s+(\d+)\s+(\d+)\s+(\d+)\s+(\d+))"};
	std::cmatch m;
	if (not std::regex_search(node.attribute("title").value(), m, re))
		throw BadRequest("(HocrPageParser) Missing bbox");
	return Box {std::stoi(m[1]), std::stoi(m[2]), std::stoi(m[3]), std::stoi(m[4])};
}

