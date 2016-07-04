#include <memory>
#include <mutex>
#include <regex>
#include <boost/log/trivial.hpp>
#include <server_http.hpp>
#include <json.hpp>
#include "db/db.hpp"
#include "db/Sessions.hpp"
#include "db/User.hpp"
#include "db/DbTableBooks.hpp"
#include "doc/Container.hpp"
#include "doc/Box.hpp"
#include "doc/Line.hpp"
#include "doc/Page.hpp"
#include "util/Config.hpp"
#include "api.hpp"

using json = nlohmann::json;

///////////////////////////////////////////////////////////////////////////////
void
pcw::GetPage::operator()(Response& res, RequestPtr req) const noexcept
{
	std::string answer, sid;
	Status status = Status::BadRequest;

	try {
		sid = getSid(req);
		BOOST_LOG_TRIVIAL(info) << " sid: " << sid;
		BOOST_LOG_TRIVIAL(info) << "book: " << req->path_match[1];
		BOOST_LOG_TRIVIAL(info) << "page: " << req->path_match[2];
		if (not sid.empty()) 
			status = doGetPage(
				sid,
				std::stoi(req->path_match[1]),
				std::stoi(req->path_match[2]),
				answer
			);
		else
			status = Status::Forbidden;
	} catch (const std::exception& e) {
		BOOST_LOG_TRIVIAL(error) << e.what();
		status = Status::InternalServerError;
	}
	reply(status, res, answer, sid);
}

///////////////////////////////////////////////////////////////////////////////
pcw::Api::Status
pcw::GetPage::doGetPage(const std::string& sid, int bookid, int pageid, std::string& answer) const
{
	auto user = sessions->find(sid);
	if (not user)
		return Status::Forbidden;
	BOOST_LOG_TRIVIAL(info) << "getPage: " << *user << ":" << sid;
	
	auto conn = connect(config_);
	DbTableBooks db(conn);
	auto page = db.getPage(user->id, bookid, pageid);
	if (not page)
		return Status::InternalServerError;

	json j;
	j["api"] = PCW_API_VERSION;
	page->store(j);
	answer = j.dump();
	return Status::Ok;
}
