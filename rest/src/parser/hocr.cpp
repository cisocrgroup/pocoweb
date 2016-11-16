#include <regex>
#include <pugixml.hpp>
#include "hocr.hpp"
#include "core/BadRequest.hpp"
#include "core/Box.hpp"

static const std::regex BBOXRE{R"(bbox\s+(\d+)\s+(\d+)\s+(\d+)\s+(\d+))"};
static const std::regex CONFRE{R"(x_wconf\s+(\d+))"};

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
Box 
pcw::hocr::get_box(pugi::xml_node node)
{
	std::cmatch m;
	if (not std::regex_search(node.attribute("title").value(), m, BBOXRE))
		throw BadRequest(
			"(HocrPageParser) Missing bbox in title: " +
			std::string(node.attribute("title").value())
		);
	return Box{
		std::stoi(m[1]), 
		std::stoi(m[2]), 
		std::stoi(m[3]), 
		std::stoi(m[4])
	};
}

////////////////////////////////////////////////////////////////////////////////
const char* 
pcw::hocr::get_cont(pugi::xml_node node)
{
	return node.child_value();
}

////////////////////////////////////////////////////////////////////////////////
double 
pcw::hocr::get_conf(pugi::xml_node node)
{
	std::cmatch m;
	if (std::regex_search(node.attribute("title").value(), m, CONFRE)) {
		auto p = static_cast<double>(std::stoi(m[1]));
		return p / 100.0;
	} else {
		return 0;
	}
}

////////////////////////////////////////////////////////////////////////////////
std::string
pcw::hocr::get_img(pugi::xml_node node)
{
	static const std::regex re{R"xx(image\s+"(.*)")xx"};
	std::cmatch m;
	if (std::regex_search(node.attribute("title").value(), m, re))
		return m[1];
	return {};
}
