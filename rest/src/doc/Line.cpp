#include <string>
#include <memory>
#include <json.hpp>
#include "Box.hpp"
#include "Line.hpp"

///////////////////////////////////////////////////////////////////////////////
void 
pcw::Line::store(nlohmann::json& json) const
{
	json["lineid"] = id;
	json["line"] = line_;
	json["cuts"] = cuts_;
	box.store(json["box"]);
}
