#include <json.hpp>
#include "BookData.hpp"

///////////////////////////////////////////////////////////////////////////////
void 
pcw::BookData::load(nlohmann::json& json)
{
	title = json["title"];
	author = json["author"];
	desc = 	json["description"];
	uri = json["uri"];
	path = 	json["path"];
	id = json["id"];
	year = 	json["year"];
	firstpage = json["firstpage"];
	lastpage = json["lastpage"];
	owner = json["ownerid"];
}

///////////////////////////////////////////////////////////////////////////////
void 
pcw::BookData::store(nlohmann::json& json) const
{
	json["title"] = title;
	json["author"] = author;
	json["description"] = desc;
	json["uri"] = uri;
	json["path"] = path;
	json["id"] = id;
	json["year"] = year;
	json["firstpage"] = firstpage;
	json["lastpage"] = lastpage;
	json["ownerid"] = owner;
}
