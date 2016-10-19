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
#include "doc/BookData.hpp"
#include "doc/Book.hpp"
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
		const auto b = req->path_match[1].length() ?
			std::stoi(req->path_match[2]): 0;
		const auto p = req->path_match[3].length() ? 
			std::stoi(req->path_match[4]) : 0;
		const auto l = req->path_match[5].length() ? 
			std::stoi(req->path_match[6]) : 0;
		BOOST_LOG_TRIVIAL(info) << " sid: " << sid;
		BOOST_LOG_TRIVIAL(info) << "book: " << b;
		BOOST_LOG_TRIVIAL(info) << "page: " << p;
		BOOST_LOG_TRIVIAL(info) << "line: " << l;
		if (not sid.empty()) {
			if (b > 0) 
				status = doGetPage(sid, b, p, l, answer);
			else 
				status = doGetBooks(sid, answer);
		} else {
			status = Status::Forbidden;
		}
	} catch (const std::exception& e) {
		BOOST_LOG_TRIVIAL(error) << e.what();
		status = Status::InternalServerError;
	}
	reply(status, res, answer, sid);
}

///////////////////////////////////////////////////////////////////////////////
pcw::Api::Status
pcw::GetPage::doGetPage(const std::string& sid, int bookid, int pageid, int lineid, std::string& answer) const
{
	auto user = sessions->find(sid);
	if (not user)
		return Status::Forbidden;
	BOOST_LOG_TRIVIAL(info) << "getPage: " << *user << ": " << sid;
	
	auto conn = connect(config_);
	DbTableBooks db(conn);
	auto book = db.get(bookid, pageid, lineid);
	if (not book)
		return Status::NotFound;

	json j;
	j["api"] = PCW_API_VERSION;
	book->store(j);
	answer = j.dump();
	return Status::Ok;
}

///////////////////////////////////////////////////////////////////////////////
pcw::Api::Status
pcw::GetPage::doGetBooks(const std::string& sid, std::string& answer) const
{
	auto user = sessions->find(sid);
	if (not user)
		return Status::Forbidden;
	
	auto conn = connect(config_);
	DbTableBooks db(conn);
	auto books = db.getAllowedBooks(user->id);
	json j;
	j["api"] = PCW_API_VERSION;
	for (const auto& book: books) {
		json jj;
		book->store(jj);
		j["books"].push_back(jj);
	}
	answer = j.dump();
	return Status::Ok;
}
	
	
