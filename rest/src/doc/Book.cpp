#include <memory>
#include <json.hpp>
#include "Box.hpp"
#include "BookData.hpp"
#include "Container.hpp"
#include "Page.hpp"
#include "Book.hpp"

///////////////////////////////////////////////////////////////////////////////
pcw::Book::Book(int iid): id(id) {assert(id > 0);}

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
pcw::Book::store(sql::Connection& c) const
{
	static const auto *sql = "UPDATE books "
				 "SET firstpage=?, lastpage=?,bookdataid=?"
				 "WHERE bookid=?";
	StatementPtr s{c.prepareStatement(sql)};
	assert(s);
	s->setInt(1, first_page_id());
	s->setInt(2, last_page_id());
	s->setInt(3, data.id);
	s->setInt(3, id);
	s->executeUpdate();	

	data.store(c);
	for (const auto& page: *this) {
		if (page)
			page->store(c, id);
	}
}

///////////////////////////////////////////////////////////////////////////////
void
pcw::Book::load(sql::Connection& c)
{
	static const auto *sql = "SELECT * FROM books WHERE bookid=?";
	StatementPtr s{c.prepareStatement(sql)};
	assert(s);
	s->setInt(1, id);
	ResultPtr res{s->executeQuery()};
	if (not res or not res->next())
		throw std::runtime_error("(Book) No such book id " + std::to_string(id));
	
	auto firstpage = res->getInt("firstpage");
	auto lastpage = res->getInt("lastpage");
	data.id = res->getInt("dataid");
	
	data.load(c);
	resize(lastpage);
	for (int i = firstpage; i > 0 and i <= lastpage; ++i) {
		(*this)[i - 1] = std::make_shared<Page>(i);
		(*this)[i - 1]->load(c, id);
	}
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

///////////////////////////////////////////////////////////////////////////////
int 
pcw::Book::first_page_id() const noexcept
{
	auto i = std::find_if(begin(*this), end(*this), [](const auto& p) {return p});
	return i != end(*this) ? i->operator*().id : 0;
}

///////////////////////////////////////////////////////////////////////////////
int 
pcw::Book::last_page_id() const noexcept
{
	auto i = std::find_if(
		std::reverse_iterator(end(*this)),
		std::reverse_iterator(begin(*this)),
		[](const auto& page) {return page;}
	);
	return i != std::reverse_iterator(begin(*this)) ? i->operator*().id : 0;
}
