#include <json.hpp>
#include "Box.hpp"

///////////////////////////////////////////////////////////////////////////////
void 
pcw::Box::store(nlohmann::json& json) const
{
	json["left"] = x0;
	json["top"] = y0;
	json["right"] = x1;
	json["bottom"] = y1;
}
