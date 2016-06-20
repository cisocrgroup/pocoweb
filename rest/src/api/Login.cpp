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
#include "util/Config.hpp"
#include "util/hash.hpp"
#include "doc/Document.hpp"
#include "db/db.hpp"
#include "db/Sessions.hpp"
#include "db/User.hpp"
#include "db/DbTableUsers.hpp"
#include "db/DbTableBooks.hpp"
#include "server_http.hpp"
#include "api.hpp"

using json = nlohmann::json;

////////////////////////////////////////////////////////////////////////////////
void
pcw::Login::operator()(Response& response, RequestPtr request) const noexcept
{
	std::string answer;
	Status status = Status::BadRequest;

	try {
		status = doLogin(
			request->path_match[1], 
			request->path_match[2], 
			answer
		);
	} catch (const std::exception& e) {
		BOOST_LOG_TRIVIAL(error) << e.what();
		answer = e.what();
	}
	reply(status, response, answer);
}

////////////////////////////////////////////////////////////////////////////////
pcw::Login::Status
pcw::Login::doLogin(const std::string& username,
		    const std::string& password,
		    std::string& answer) const
{
	auto conn = connect(config_);
	DbTableUsers users{conn};
	auto user = users.findUserByNameOrEmail(username); 

	if (not user) // invalid user / email
		return Status::Forbidden;
	if (not users.authenticate(*user, password))
		return Status::Forbidden;
	assert(user);
	
	std::string sid;
	do {
		sid = gensessionid(42);	
	} while (not sessions->insert(sid, user));
	
	json j;
	j["api"] = PCW_API_VERSION;
	j["sessionid"] = sid;
	j["user"]["name"] = user->name;
	j["user"]["email"] = user->email;
	j["user"]["institute"] = user->institute;
	
	DbTableBooks books{conn};
	const auto data = books.getAllowedBooks(user->id);
	for (const auto& d: data) {
		json jj;
		jj["title"] = d.second.title;
		jj["author"] = d.second.author;
		jj["description"] = d.second.desc;
		jj["uri"] = d.second.uri;
		jj["year"] = d.second.year;
		jj["bookdataid"] = d.second.id;
		jj["bookid"] = d.first;
		j["books"].push_back(jj);
	}
	BOOST_LOG_TRIVIAL(info) << *user << ": " << sid << " logged on";
	answer = j.dump();
	return Status::Ok;
}
