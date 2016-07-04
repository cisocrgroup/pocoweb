#include <memory>
#include <json.hpp>
#include "Box.hpp"
#include "Line.hpp"
#include "Container.hpp"
#include "Page.hpp"

///////////////////////////////////////////////////////////////////////////////
void 
pcw::Page::store(nlohmann::json& json) const
{
	json["pageid"] = id;
	json["image"] = image;
	box.store(json["box"]);
	for (const auto& line: *this) {
		assert(line);
		nlohmann::json tmp;
		line->store(tmp);
		json["lines"].push_back(tmp);
	} 
}
