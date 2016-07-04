#include <json.hpp>
#include "Box.hpp"

///////////////////////////////////////////////////////////////////////////////
void
pcw::Box::load(nlohmann::json& json)
{
	x0 = json["left"];
	y0 = json["top"];
	x1 = json["right"];
	y1 = json["bottom"];
}

///////////////////////////////////////////////////////////////////////////////
void 
pcw::Box::store(nlohmann::json& json) const
{
	json["left"] = x0;
	json["top"] = y0;
	json["right"] = x1;
	json["bottom"] = y1;
}
