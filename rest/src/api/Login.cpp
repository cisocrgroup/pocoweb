#include <mutex>
#include <iostream>
#include <string>
#include <regex>
#include <boost/log/trivial.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <cppconn/statement.h>
#include <mysql_connection.h>
#include <json.hpp>
#include "Config.hpp"
#include "util/hash.hpp"
#include "db/db.hpp"
#include "db/Sessions.hpp"
#include "db/User.hpp"
#include "db/DbTableUsers.hpp"
#include "db/DbTableBooks.hpp"
#include "doc/BookData.hpp"
#include "doc/Box.hpp"
#include "doc/Container.hpp"
#include "doc/Book.hpp"
#include "doc/Line.hpp"
#include "doc/Page.hpp"
#include "server_http.hpp"
#include "api.hpp"

using json = nlohmann::json;

////////////////////////////////////////////////////////////////////////////////
void
pcw::Login::operator()(Response& response, RequestPtr request) const noexcept
{
	std::string answer, sid;
	Status status = Status::BadRequest;

	try {
		status = doLogin(
			request->path_match[1], 
			request->path_match[2], 
			sid,
			answer
		);
	} catch (const std::exception& e) {
		BOOST_LOG_TRIVIAL(error) << e.what();
		status = Status::InternalServerError;
	}
	reply(status, response, answer, sid);
}

////////////////////////////////////////////////////////////////////////////////
pcw::Login::Status
pcw::Login::doLogin(const std::string& username,
		    const std::string& password,
		    std::string& sid,
		    std::string& answer) const
{
	auto conn = connect(config_);
	const auto user = User::create(*conn, username);

	if (not user) // invalid user / email
		return Status::Forbidden;
	if (not user->authenticate(*conn, password))
		return Status::Forbidden;
	assert(user);
	
	do {
		sid = gensessionid(16);	
	} while (not sessions->insert(sid, user));
	
	json j;
	j["api"] = PCW_API_VERSION;
	j["user"] = user->json();
	
	DbTableBooks books{conn};
	const auto allowedBooks = books.getAllowedBooks(user->id);
	for (const auto& book: allowedBooks) {
		json jj;
		book->store(jj);
		j["books"].push_back(jj);
	}
	BOOST_LOG_TRIVIAL(info) << *user << ": " << sid << " logged on";
	answer = j.dump();
	return Status::Ok;
}
