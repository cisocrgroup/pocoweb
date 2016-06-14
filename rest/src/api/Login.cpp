#include <iostream>
#include <string>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include "util/Config.hpp"
#include "server_http.hpp"
#include "api.hpp"

namespace pt = boost::property_tree;

////////////////////////////////////////////////////////////////////////////////
void
pcw::Login::operator()(Response& response, RequestPtr request) const noexcept
{
	//	auto content = request->content.string();
	//	std::cerr << "CONTENT: " << content << "\n";
	pt::ptree json;
	pt::read_json(request->content, json);
	std::cerr << "user = " << json.get<std::string>("user")
		  << "\npass = " << json.get<std::string>("pass") << "\n";
	//	std::cout << "CONTENT: " << content << "\n";
	response << "HTTP/1.1 200 OK\r\nContent-Length: "
		 << 2 << "\r\n\r\n"
		 << "ok";
}
