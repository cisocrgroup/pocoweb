#include <json.hpp>
#include "BookData.hpp"

///////////////////////////////////////////////////////////////////////////////
void
pcw::BookData::store(sql::Connection& c) const
{
	static const char *sql = "UPDATE bookdata "
				 "SET owner=?,year=?,title=?,author=?,"
				 "description=?,uri=?,directory=? "
				 "WHERE bookdataid=?";
	StatementPtr s{c.prepareStatement(sql)};
	assert(s);
	s->setInt(1, owner);
	s->setInt(2, year);
	s->setString(3, title);
	s->setString(4, author);
	s->setString(5, description);
	s->setString(6, uri);
	s->setString(7, path.string());
	s->setInt(8, id);
	s->executeUpdate();
}

///////////////////////////////////////////////////////////////////////////////
void
pcw::BookData::load(sql::Connection& c) 
{
	static const char *sql = "SELECT * FROM bookdata WHERE bookdataid=?";
	StatementPtr s{c.prepareStatement(sql)};
	assert(s);
	s->setInt(1, id);
	ResultPtr res{s->executeQuery()};
	if (not res or not res->next()) 
		throw std::runtime_error("(BookData) No such book data id " + std::to_string(id));
	owner = s->getInt("owner");
	year = s->getInt("year");
	title = s->getString("title");
	author = s->getString("author");
	description = s->getString("description");
	uri = s->getString("uri");
	path = s->getString("directory");
}

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
