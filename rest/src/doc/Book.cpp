#include <memory>
#include <json.hpp>
#include "Box.hpp"
#include "BookData.hpp"
#include "Container.hpp"
#include "Page.hpp"
#include "Book.hpp"

///////////////////////////////////////////////////////////////////////////////
pcw::PagePtr 
pcw::Book::add_page(PagePtr page)
{
	if (not page or page->id < 1) // invalid page or invalid page id
		return nullptr;
	if (size() < static_cast<size_t>(page->id))
		resize(page->id);
	(*this)[page->id - 1] = page;
	return (*this)[page->id - 1];
}

///////////////////////////////////////////////////////////////////////////////
pcw::PagePtr 
pcw::Book::get_page(int id) const noexcept
{
	if (id > 0 and static_cast<size_t>(id) <= size()) {
		return (*this)[id - 1];
	}
	return nullptr;
}

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
