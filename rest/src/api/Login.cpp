#include <iostream>
#include <string>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/core.hpp>
#include "util/Config.hpp"
#include "server_http.hpp"
#include "api.hpp"

namespace pt = boost::property_tree;

////////////////////////////////////////////////////////////////////////////////
void
pcw::Login::operator()(Response& response, RequestPtr request) const noexcept
{
	try {
		BOOST_LOG_TRIVIAL(error) << "Logging";
		pt::ptree json;
		pt::read_json(request->content, json);
		ok(response, "{\"status\": \"ok\"}");
	} catch (const std::exception& e) {
		BOOST_LOG_TRIVIAL(error) << e.what();
		badRequest(response, e.what());
	}
}
