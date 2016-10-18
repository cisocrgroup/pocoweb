#include <regex>
#include <cppconn/connection.h>
#include "Database.hpp"
#include "Sessions.hpp"
#include "Route.hpp"

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
static const std::string COOKIE{"Cookie"};

////////////////////////////////////////////////////////////////////////////////
std::string
get_session_id(const crow::request& request) noexcept
{
	static const std::regex sid{R"(pcwsid=([0-9a-zA-Z]+);?)"};
	auto range = request.headers.equal_range(COOKIE);
	std::smatch m;
	for (auto i = range.first; i != range.second; ++i) {
		if (std::regex_match(i->second, m, sid))
			return m[1];
	}
	return "";
}

////////////////////////////////////////////////////////////////////////////////
void 
set_session_id(crow::response& response, const std::string& sid) noexcept
{
	response.add_header(COOKIE, sid);
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
