#include <memory>
#include <mutex>
#include <regex>
#include <boost/log/trivial.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <server_http.hpp>
#include <json.hpp>
#include "doc/Document.hpp"
#include "db/db.hpp"
#include "db/Sessions.hpp"
#include "db/User.hpp"
#include "db/DbTableBooks.hpp"
#include "util/Config.hpp"
#include "api.hpp"

using json = nlohmann::json;

///////////////////////////////////////////////////////////////////////////////
void
pcw::GetPage::operator()(Response& res, RequestPtr req) const noexcept
{
	static const std::regex session{R"(sessionid=([a-fA-F0-9]+))"};
	static const std::regex bookid{R"(bookid=(\d+))"};
	static const std::regex pageid{R"(pageid=(\d+))"};
	std::string answer;
	Status status = Status::BadRequest;
	BOOST_LOG_TRIVIAL(info) << "HERE";

	try {
		const auto m = req->path_match[1];
		std::smatch m1, m2, m3;
	
		if (std::regex_search(m.first, m.second, m1, session) and
		    std::regex_search(m.first, m.second, m2, bookid) and
		    std::regex_search(m.first, m.second, m3, pageid)) 
			status = doGetPage(m1[1], std::stoi(m2[1]), std::stoi(m3[1]), answer);
	} catch (const std::exception& e) {
		BOOST_LOG_TRIVIAL(error) << e.what();
		status = Status::InternalServerError;
	}
	reply(status, res, answer);
}

///////////////////////////////////////////////////////////////////////////////
pcw::Api::Status
pcw::GetPage::doGetPage(const std::string& sid, int bookid, int pageid, std::string& answer) const
{
	BOOST_LOG_TRIVIAL(info) << "sid: " << sid << "bookid: " << bookid << " pageid: " << pageid;
	auto user = sessions->find(sid);
	if (not user)
		return Status::Forbidden;
	BOOST_LOG_TRIVIAL(info) << "after user";
	
	auto conn = connect(config_);
	DbTableBooks db(conn);
	auto page = db.getPage(*user, bookid, pageid);
	if (not page)
		return Status::InternalServerError;
	BOOST_LOG_TRIVIAL(info) << "after page";

	json j;
	j["bookid"] = bookid;
	j["pageid"] = pageid;
	
	for (const auto& l: *page) {
		json jj;
		jj["box"]["x0"] = l->box.x0;
		jj["box"]["y0"] = l->box.y0;
		jj["box"]["x1"] = l->box.x1;
		jj["box"]["y1"] = l->box.y1;
		jj["lineid"] = l->id;
		jj["line"] = l->line();
		jj["cuts"] = l->cuts();
		j["lines"].push_back(jj);
	}
	answer = j.dump();
	return Status::Ok;
}
