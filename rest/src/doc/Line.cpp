#include <string>
#include <memory>
#include <json.hpp>
#include <cppconn/connection.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include "db/db.hpp"
#include "Box.hpp"
#include "Line.hpp"

///////////////////////////////////////////////////////////////////////////////
pcw::Line::Line(int iid): id(iid) {assert(id > 0);}

///////////////////////////////////////////////////////////////////////////////
void 
pcw::Line::dbload(sql::Connection& c, int bookid, int pageid)
{
	static const char *sql = "SELECT * FROM linesx "
				 "WHERE bookid=? AND pageid=? AND lineid=?";
	PreparedStatementPtr s{c.prepareStatement(sql)};
	s->setInt(1, bookid);
	s->setInt(2, pageid);
	s->setInt(3, id);
	
	ResultSetPtr res{s->executeQuery()};
	if (not res or not res->next()) 
		throw std::runtime_error(
			"(Line) No such line " +
			std::to_string(bookid) + " " + 
			std::to_string(pageid) + " " + 
			std::to_string(id)
		);
	line_ = res->getString("lstr");
	cuts_ = cuts_from_str(res->getString("cuts"));
	box.left = res->getInt("lleft");
	box.right = res->getInt("lright");
	box.top = res->getInt("ltop");
	box.bottom = res->getInt("lbottom");
	imagefile = res->getString("imagepath");
}

///////////////////////////////////////////////////////////////////////////////
void 
pcw::Line::dbstore(sql::Connection& c, int bookid, int pageid) const
{
	static const char *sql = "INSERT INTO linesx "
				 "(bookid,pageid,lineid,imagepath,lstr,cuts,pleft,pright,ptop,pbottom) "
				 "VALUES (?,?,?,?,?,?,?,?,?,?)";
	PreparedStatementPtr s{c.prepareStatement(sql)};
	s->setInt(1, bookid);
	s->setInt(2, pageid);
	s->setInt(3, id);
	s->setString(4, imagefile.string());
	s->setString(5, line_);
	s->setString(6, cuts_str());
	s->setInt(7, box.left);
	s->setInt(8, box.right);
	s->setInt(9, box.top);
	s->setInt(10, box.bottom);
	s->executeUpdate();
}

///////////////////////////////////////////////////////////////////////////////
void
pcw::Line::load(nlohmann::json& json)
{
	id = json["lineid"];
	line_ = json["line"];
	cuts_.clear();
	for (const auto& cut: json["cuts"])
		cuts_.push_back(cut);
	box.load(json["box"]);
}

///////////////////////////////////////////////////////////////////////////////
void 
pcw::Line::store(nlohmann::json& json) const
{
	json["lineid"] = id;
	json["line"] = line_;
	json["cuts"] = cuts_;
	box.store(json["box"]);
}

///////////////////////////////////////////////////////////////////////////////
std::string 
pcw::Line::cuts_str() const noexcept
{
	// using std::begin;
	// using std::end;
	std::stringstream os;
	std::copy(begin(cuts_), end(cuts_), std::ostream_iterator<int>(os, " "));
	return os.str();
}

///////////////////////////////////////////////////////////////////////////////
pcw::Line::Cuts
pcw::Line::cuts_from_str(const std::string& str)
{
	std::stringstream is(str);
	Cuts cuts;
	std::copy(
		std::istream_iterator<int>(is), 
		std::istream_iterator<int>(),
		std::back_inserter(cuts)
	);
	return cuts;
}
