#include <memory>
#include <json.hpp>
#include "cppconn/connection.h"
#include <cppconn/prepared_statement.h>
#include "db/db.hpp"
#include "Box.hpp"
#include "BookData.hpp"
#include "Container.hpp"
#include "Page.hpp"
#include "Book.hpp"

///////////////////////////////////////////////////////////////////////////////
pcw::Book::Book(int iid): id(iid) {assert(id > 0);}

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
pcw::Book::dbstore(sql::Connection& c) const
{
	static const auto *sql = "UPDATE books "
				 "SET firstpage=?, lastpage=?,bookdataid=?"
				 "WHERE bookid=?";
	PreparedStatementPtr s{c.prepareStatement(sql)};
	assert(s);
	s->setInt(1, first_page_id());
	s->setInt(2, last_page_id());
	s->setInt(3, data.id);
	s->setInt(3, id);
	s->executeUpdate();	
	data.dbstore(c);

	// do not store pages (they have to be created serparately)
}

///////////////////////////////////////////////////////////////////////////////
void
pcw::Book::dbload(sql::Connection& c)
{
	static const auto* sql = "SELECT * FROM books NATURAL JOIN bookdata WHERE bookid=?";
	PreparedStatementPtr s{c.prepareStatement(sql)};
	assert(s);
	s->setInt(1, id);
	ResultSetPtr res{s->executeQuery()};
	if (not res or not res->next())
		throw std::runtime_error("(Book) No such book id " + std::to_string(id));
	
	auto firstpage = res->getInt("firstpage");
	auto lastpage = res->getInt("lastpage");
	data.id = res->getInt("dataid");
	
	// load bookdata
	data.owner = res->getInt("owner");
	data.year = res->getInt("year");
	data.title = res->getString("title");
	data.author = res->getString("author");
	data.desc = res->getString("description");
	data.uri = res->getString("uri");
	data.path = res->getString("directory");

	resize(lastpage);
	for (int i = firstpage; i > 0 and i <= lastpage; ++i) {
		auto page = std::make_shared<Page>(i);
		if (page->dbload(c, id))
			(*this)[i - 1] = page;
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
	using std::begin;
	using std::end;
	const auto e = end(*this);
	auto i = std::find_if(begin(*this), e, [](const auto& p) {return p;});
	return i != e ? i->operator*().id : 0;
}

///////////////////////////////////////////////////////////////////////////////
int 
pcw::Book::last_page_id() const noexcept
{
	using std::begin;
	using std::end;
	const auto b = std::make_reverse_iterator(end(*this));
	const auto e = std::make_reverse_iterator(begin(*this));
	auto i = std::find_if(b, e, [](const auto& page) {return page;});
	return i != e ? i->operator*().id : 0;
}
