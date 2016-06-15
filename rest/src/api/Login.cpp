#include <iostream>
#include <string>
#include <regex>
#include <boost/log/trivial.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <cppconn/statement.h>
#include <mysql_connection.h>
#include "util/Config.hpp"
#include "util/hash.hpp"
#include "db/db.hpp"
#include "db/User.hpp"
#include "db/DbTableUsers.hpp"
#include "server_http.hpp"
#include "api.hpp"

namespace pt = boost::property_tree;

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
		    std::string& answer) const noexcept
{
	try {
		auto conn = connect(config_);
		DbTableUsers db{conn};
		auto user = db.findUserByName(username); // try name
		if (not user) // try email ...
			user = db.findUserByEmail(username);
		if (not user) // invalid user / email
			return Status::Forbidden;
		if (not db.authenticate(*user, password))
			return Status::Forbidden;
		return session(answer);
	} catch (const std::exception& e) {
		BOOST_LOG_TRIVIAL(error) << e.what();
	}
	return Status::InternalServerError;
}

////////////////////////////////////////////////////////////////////////////////
pcw::Api::Status
pcw::Login::session(std::string& answer) const noexcept
{
	try {
		auto id = gensessionid(42);
		pt::ptree json;
		json.put("sessionid", id);
		std::stringstream ios;
		pt::write_json(ios, json);
		answer = ios.str();
		return Status::Ok;
	} catch (const std::exception& e) {
		BOOST_LOG_TRIVIAL(error) << e.what();
		return Status::InternalServerError;
	}
}
