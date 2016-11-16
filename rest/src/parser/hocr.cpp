#include <regex>
#include <pugixml.hpp>
#include <utf8.h>
#include "hocr.hpp"
#include "core/BadRequest.hpp"
#include "core/Box.hpp"

static const std::regex BBOXRE{R"(bbox\s+(\d+)\s+(\d+)\s+(\d+)\s+(\d+))"};
static const std::regex CONFRE{R"(x_wconf\s+(\d+))"};
static const std::regex IMGRE{R"xx(image\s+"(.*)")xx"};

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
std::wstring
pcw::hocr::get_cont(pugi::xml_node node)
{
	const auto cont = node.child_value();
	const auto n = strlen(cont);
	std::wstring res;
	res.reserve(n);
	utf8::utf8to32(cont, cont + n, std::back_inserter(res));
	return res;
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
	std::cmatch m;
	if (std::regex_search(node.attribute("title").value(), m, IMGRE))
		return m[1];
	return {};
}
////////////////////////////////////////////////////////////////////////////////
void 
pcw::hocr::set_box(const Box& box, pugi::xml_node node)
{
	std::stringstream os;
	os << "bbox " 
	   << box.left() << " " 
	   << box.top() << " "
	   << box.right() << " "
	   << box.bottom();
	if (not node.attribute("title")) {
		node.append_attribute("title").set_value(os.str().data());
	} else {
		std::string title = node.attribute("title").value();
		auto repl = std::regex_replace(title, BBOXRE, os.str());
		node.attribute("title").set_value(repl.data());
	}
}

////////////////////////////////////////////////////////////////////////////////
void 
pcw::hocr::set_cont(const std::wstring& cont, pugi::xml_node node)
{
	if (not node.first_child())
		node.append_child(pugi::node_pcdata);
	std::string ustr;
	ustr.reserve(cont.size());
	utf8::utf32to8(begin(cont), end(cont), std::back_inserter(ustr));
	node.first_child().set_value(ustr.data());
}

////////////////////////////////////////////////////////////////////////////////
void 
pcw::hocr::set_conf(double conf, pugi::xml_node node)
{
	auto iconf = static_cast<int>(conf * 100);
	std::stringstream os;
	os << "x_wconf " << iconf;
	if (not node.attribute("title")) {
		node.append_attribute("title").set_value(os.str().data());
	} else {
		std::string title = node.attribute("title").value();
		auto repl = std::regex_replace(title, CONFRE, os.str());
		node.attribute("title").set_value(repl.data());
	}
}

////////////////////////////////////////////////////////////////////////////////
void 
pcw::hocr::set_img(const std::string& str, pugi::xml_node node)
{
	if (not node.attribute("title")) {
		node.append_attribute("title").set_value(str.data());
	} else {
		std::string title = node.attribute("title").value();
		auto repl = std::regex_replace(title, IMGRE, str);
		node.attribute("title").set_value(repl.data());
	}
}
