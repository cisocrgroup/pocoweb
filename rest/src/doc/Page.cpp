#include <memory>
#include <json.hpp>
#include <cppconn/connection.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include "db/db.hpp"
#include "hocr.hpp"
#include "Box.hpp"
#include "Line.hpp"
#include "Container.hpp"
#include "Page.hpp"

///////////////////////////////////////////////////////////////////////////////
pcw::Page::Page(int iid): id(iid) {assert(id > 0);}

///////////////////////////////////////////////////////////////////////////////
void 
pcw::Page::parse()
{
	// TODO: add handling for hocr, abbyy, ocropus
	parse_hocr_page(*this);
}

///////////////////////////////////////////////////////////////////////////////
bool 
pcw::Page::dbload(sql::Connection& c, int bookid)
{
	static const char *sql = "SELECT * FROM page WHERE bookid=? AND pageid=?";
	PreparedStatementPtr s{c.prepareStatement(sql)};
	s->setInt(1, bookid);
	s->setInt(2, id);
	ResultSetPtr res{s->executeQuery()};
	if (not res or not res->next())
		return false;
	
	const auto n = res->getInt("nlines");
	imagefile = res->getString("imagepath");
	ocrfile = res->getString("ocrpath");
	box.left = res->getInt("pleft");
	box.right = res->getInt("pright");
	box.top = res->getInt("ptop");
	box.bottom = res->getInt("pbottom");

	resize(n);
	for (int i = 0; i < n; ++i) {
		(*this)[i] = std::make_shared<Line>(i + 1);
		(*this)[i]->dbload(c, bookid, id);
	}
	return true;
}

///////////////////////////////////////////////////////////////////////////////
void 
pcw::Page::dbstore(sql::Connection& c, int bookid) const
{
	static const char *sql = "INSERT INTO pages "
				 "(bookid,pageid,nlines,imagepath,ocrpath,pleft,pright,ptop,pbottom) "
				 "VALUES (?,?,?,?,?,?,?,?,?)";
	PreparedStatementPtr s{c.prepareStatement(sql)};
	s->setInt(1, bookid);
	s->setInt(2, id);
	s->setInt(3, static_cast<int>(size()));
	s->setString(4, imagefile.string());
	s->setString(5, ocrfile.string());
	s->setInt(6, box.left);
	s->setInt(7, box.right);
	s->setInt(8, box.top);
	s->setInt(9, box.bottom);
	s->executeUpdate();

	for (const auto& line: *this)
		line->dbstore(c, bookid, id);
}

///////////////////////////////////////////////////////////////////////////////
void 
pcw::Page::load(nlohmann::json& json)
{
	id = json["pageid"];
	const std::string a = json["imagefile"];
	const std::string b = json["ocrfile"];
	imagefile = a;
	ocrfile = b;
	box.load(json["box"]);
	clear();
	for (auto& line: json["lines"])
		push_back(std::make_shared<Line>(line));
}

///////////////////////////////////////////////////////////////////////////////
void 
pcw::Page::store(nlohmann::json& json) const
{
	json["pageid"] = id;
	json["imagefile"] = imagefile.string();
	json["ocrfile"] = ocrfile.string();
	box.store(json["box"]);
	for (const auto& line: *this) {
		assert(line);
		nlohmann::json tmp;
		line->store(tmp);
		json["lines"].push_back(tmp);
	} 
}
