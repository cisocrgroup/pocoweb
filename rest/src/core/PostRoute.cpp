#include <crow.h>
#include <regex>
#include "BadRequest.hpp"
#include "PostRoute.hpp"

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
std::string 
PostRoute::extract_content(const crow::request& request)
{
	static const std::string ContentType{"Content-Type"};
	static const std::regex BoundaryRegex{R"(boundary=(.*);?$)"};
	CROW_LOG_INFO << "### BODY ###\n" << request.body;

	std::smatch m;
	if (std::regex_search(request.get_header_value(ContentType), m, BoundaryRegex))
		return extract_multipart(request, m[1]);
	else 
		return extract_raw(request);
}

////////////////////////////////////////////////////////////////////////////////
std::string 
PostRoute::extract_multipart(const crow::request& request, const std::string& boundary)
{
	CROW_LOG_INFO << "(PostRoute) Boundary in multipart data: " << boundary;
	auto b = request.body.find("\r\n\r\n");
	if (b == std::string::npos)
		throw BadRequest("(PostRoute) Missing '\\r\\n\\r\\n' in multipart data");
	b += 4;
	auto e = request.body.rfind(boundary);
	if (b == std::string::npos)
		throw BadRequest("(PostRoute) Boundary in multipart data" + boundary);
	if (e < b)
		throw BadRequest("(PostRoute) Invalid Boundary in multipart data" + boundary);
	CROW_LOG_INFO << "(PostRoute) b = " << b << ", e = " << e;
	
	return request.body.substr(b, e - b);
}

////////////////////////////////////////////////////////////////////////////////
std::string 
PostRoute::extract_raw(const crow::request& request)
{
	return request.body;
}

