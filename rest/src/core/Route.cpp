#include <chrono>
#include <ctime>
#include <sstream>
#include <regex>
#include <cppconn/connection.h>
#include <crow/logging.h>
#include "utils/Error.hpp"
#include "Book.hpp"
#include "Page.hpp"
#include "Line.hpp"
#include "User.hpp"
#include "Cache.hpp"
#include "AppCache.hpp"
#include "Database.hpp"
#include "Sessions.hpp"
#include "Route.hpp"

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
std::string
pcw::get_session_id(const crow::request& request) noexcept
{
	static const std::string Cookie{"Cookie"};
	static const std::regex sid{R"(pcwsid=([0-9a-zA-Z]+);?)"};
	auto range = request.headers.equal_range(Cookie);
	std::smatch m;
	for (auto i = range.first; i != range.second; ++i) {
		if (std::regex_match(i->second, m, sid))
			return m[1];
	}
	return "";
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
Route::session(const crow::request& request) const noexcept
{
	return sessions().find_session(get_session_id(request));
}

////////////////////////////////////////////////////////////////////////////////
Database
Route::database(const crow::request& request) const
{
	return database(session(request));
}

////////////////////////////////////////////////////////////////////////////////
Database
Route::database(SessionPtr session) const
{
	assert(config_);
	if (not session)
		THROW(Forbidden);
	return Database{session, config_, cache_};
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

////////////////////////////////////////////////////////////////////////////////
BookViewPtr
Route::find(const Database& db, int bid) const
{
	auto book = db.select_project(bid);
	if (not book)
		THROW(NotFound, "Not Found: book id ", bid);
	return book;
}

////////////////////////////////////////////////////////////////////////////////
PagePtr
Route::find(const Database& db, int bid, int pid) const
{
	auto page = find(db, bid)->find(pid);
	if (not page)
		THROW(NotFound, "Not found: book id ", bid, ", page id ", pid);
	return page;
}

////////////////////////////////////////////////////////////////////////////////
LinePtr
Route::find(const Database& db, int bid, int pid, int lid) const
{
	auto page = find(db, bid, pid);
	assert(page);
	auto line = page->find(lid);
	if (not line)
		THROW(NotFound, "Not found: book id ", bid, ", page id ", pid, ", line id ", lid);
	return line;
}
