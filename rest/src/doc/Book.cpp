#include <memory>
#include <json.hpp>
#include "Box.hpp"
#include "BookData.hpp"
#include "Container.hpp"
#include "Page.hpp"
#include "Book.hpp"

///////////////////////////////////////////////////////////////////////////////
void 
pcw::Book::load(nlohmann::json& json) 
{
	id = json["bookid"];
	data.load(json["data"]);
	for (auto& page: json["pages"])
		push_back(std::make_shared<Page>(page));
}
		
///////////////////////////////////////////////////////////////////////////////
void 
pcw::Book::store(nlohmann::json& json) const
{
	json["bookid"] = id;
	data.store(json["data"]);
	for (const auto& page: *this) {
		assert(page);
		nlohmann::json tmp;
		page->store(tmp);
		json["pages"].push_back(tmp);
	}
}
