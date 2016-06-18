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
#include "db/db.hpp"
#include "db/Sessions.hpp"
#include "db/User.hpp"
#include "db/DbTableUsers.hpp"
#include "server_http.hpp"
#include "api.hpp"

using json = nlohmann::json;

////////////////////////////////////////////////////////////////////////////////
void
pcw::Login::operator()(Response& response, RequestPtr request) const noexcept
{
	static const std::regex username{R"(username=([^&]+))"};
	static const std::regex password{R"(password=([^&]+))"};
	std::string answer;
	Status status = Status::BadRequest;

	try {
		const auto m = request->path_match[1];
		std::smatch m1, m2;

		if (std::regex_search(m.first, m.second, m1, username) and
		    std::regex_search(m.first, m.second, m2, password)) {
			status = doLogin(m1[1], m2[1], answer);
		}
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
	DbTableUsers db{conn};
	auto user = db.findUserByNameOrEmail(username); 

	if (not user) // invalid user / email
		return Status::Forbidden;
	if (not db.authenticate(*user, password))
		return Status::Forbidden;
	assert(user);
	createSessionAndWrite(*user, answer);
	return Status::Ok;
}

////////////////////////////////////////////////////////////////////////////////
void
pcw::Login::createSessionAndWrite(User& user, std::string& answer) const
{
	std::string sid;
	do {
		sid = gensessionid(42);
	} while (not sessions->insert(sid, user.shared_from_this()));
	json j;
	j["sessionid"] = sid;
	j["user"]["name"] = user.name;
	j["user"]["email"] = user.email;
	j["user"]["institute"] = user.institute;
	BOOST_LOG_TRIVIAL(info) << user << ": " << sid << " logged on";
	answer = j.dump();
}
