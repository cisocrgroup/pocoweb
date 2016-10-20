#include <json.hpp>
#include "Box.hpp"

///////////////////////////////////////////////////////////////////////////////
void
pcw::Box::load(nlohmann::json& json)
{
	left = json["left"];
	top = json["top"];
	right = json["right"];
	bottom = json["bottom"];
}

///////////////////////////////////////////////////////////////////////////////
void 
pcw::Box::store(nlohmann::json& json) const
{
	json["left"] = left;
	json["top"] = top;
	json["right"] = right;
	json["bottom"] = bottom;
}
