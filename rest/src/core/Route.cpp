#include <regex>
#include <cppconn/connection.h>
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
	response.add_header(SetCookie, "pcwsid=" + sid);
}

////////////////////////////////////////////////////////////////////////////////
SessionPtr
Route::session(const crow::request& request) const noexcept
{
	return sessions().find_session(get_session_id(request));
}

////////////////////////////////////////////////////////////////////////////////
boost::optional<Database> 
Route::database(const crow::request& request) const noexcept
{
	return database(session(request));
}

////////////////////////////////////////////////////////////////////////////////
boost::optional<Database> 
Route::database(SessionPtr session) const noexcept
{
	assert(config_);
	return session ? Database(session, config_) : boost::optional<Database>{};
}
