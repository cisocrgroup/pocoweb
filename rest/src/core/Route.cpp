#include <chrono>
#include <ctime>
#include <sstream>
#include <regex>
#include <crow/logging.h>
#include "AppCache.hpp"
#include "Book.hpp"
#include "Cache.hpp"
#include "Line.hpp"
#include "Page.hpp"
#include "Route.hpp"
#include "Session.hpp"
#include "SessionStore.hpp"
#include "User.hpp"
#include "utils/Error.hpp"

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
boost::optional<std::string>
pcw::get_session_id(const crow::request& request) noexcept
{
	static const std::string Cookie{"Cookie"};
	static const std::regex sid{R"(pcwsid=([0-9a-zA-Z]+);?)"};
	auto range = request.headers.equal_range(Cookie);
	std::smatch m;
	for (auto i = range.first; i != range.second; ++i) {
		if (std::regex_match(i->second, m, sid))
			return std::string(m[1]);
	}
	return {};
}

////////////////////////////////////////////////////////////////////////////////
void
pcw::set_session_id(crow::response& response, const std::string& sid) noexcept
{
	static const std::string SetCookie{"Set-Cookie"};
	using namespace std::chrono;
	using namespace std::chrono_literals;
	system_clock::time_point now(system_clock::now());
	auto expires = system_clock::to_time_t(now + 23h);
	std::ostringstream os;
	os << "pcwsid=" << sid << "; path=*; Expires=" << std::ctime(&expires) << ";";
	response.add_header(SetCookie, os.str());
}

////////////////////////////////////////////////////////////////////////////////
Route::~Route() noexcept
{

}

////////////////////////////////////////////////////////////////////////////////
SessionPtr
Route::new_session(const User& user) const noexcept
{
	assert(session_store_);
	return session_store_->new_session(user, cache_);
}

////////////////////////////////////////////////////////////////////////////////
SessionPtr
Route::session(const crow::request& request) const noexcept
{
	auto sid = get_session_id(request);
	if (not sid)
		THROW(Forbidden);
	assert(session_store_);
	auto session = session_store_->find_session(*sid);
	if (not session)
		THROW(BadRequest);
	return session;
}

////////////////////////////////////////////////////////////////////////////////
std::string
Route::extract_content(const crow::request& request)
{
	static const std::string ContentType{"Content-Type"};
	static const std::regex BoundaryRegex{R"(boundary=(.*);?$)"};
	// CROW_LOG_INFO << "### BODY ###\n" << request.body;

	std::smatch m;
	if (std::regex_search(request.get_header_value(ContentType), m, BoundaryRegex))
		return extract_multipart(request, m[1]);
	else
		return extract_raw(request);
}

////////////////////////////////////////////////////////////////////////////////
std::string
Route::extract_multipart(const crow::request& request, const std::string& boundary)
{
	CROW_LOG_INFO << "(Route) Boundary in multipart data: " << boundary;
	auto b = request.body.find("\r\n\r\n");
	if (b == std::string::npos)
		THROW(BadRequest, "(Route) Missing '\\r\\n\\r\\n' in multipart data");
	b += 4;
	auto e = request.body.rfind(boundary);
	if (b == std::string::npos)
		THROW(BadRequest, "(Route) Boundary in multipart data ", boundary);
	if (e < b)
		THROW(BadRequest, "(Route) Boundary in multipart data ", boundary);
	CROW_LOG_INFO << "(Route) b = " << b << ", e = " << e;

	return request.body.substr(b, e - b);
}

////////////////////////////////////////////////////////////////////////////////
std::string
Route::extract_raw(const crow::request& request)
{
	return request.body;
}

