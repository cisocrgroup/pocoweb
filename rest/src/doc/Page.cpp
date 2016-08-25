#include <memory>
#include <json.hpp>
#include "hocr.hpp"
#include "Box.hpp"
#include "Line.hpp"
#include "Container.hpp"
#include "Page.hpp"

///////////////////////////////////////////////////////////////////////////////
void 
pcw::Page::parse()
{
	// TODO: add handling for hocr, abbyy, ocropus
	parse_hocr_page(*this);
}

///////////////////////////////////////////////////////////////////////////////
void 
pcw::Page::load(nlohmann::json& json)
{
	id = json["pageid"];
	const std::string a = json["imagefile"];
	const std::string b = json["ocrfile"];
	imagefile = a;
	ocrfile = b;
	box.load(json["box"]);
	clear();
	for (auto& line: json["lines"])
		push_back(std::make_shared<Line>(line));
}

///////////////////////////////////////////////////////////////////////////////
void 
pcw::Page::store(nlohmann::json& json) const
{
	json["pageid"] = id;
	json["imagefile"] = imagefile.string();
	json["ocrfile"] = ocrfile.string();
	box.store(json["box"]);
	for (const auto& line: *this) {
		assert(line);
		nlohmann::json tmp;
		line->store(tmp);
		json["lines"].push_back(tmp);
	} 
}
