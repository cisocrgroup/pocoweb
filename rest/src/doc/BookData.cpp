#include <json.hpp>
#include "BookData.hpp"

///////////////////////////////////////////////////////////////////////////////
void 
pcw::BookData::store(nlohmann::json& json) const
{
	json["title"] = title;
	json["author"] = author;
	json["description"] = desc;
	json["uri"] = uri;
	json["path"] = path;
	json["bookid"] = id;
	json["year"] = year;
	json["firstpage"] = firstpage;
	json["lastpage"] = lastpage;
}
