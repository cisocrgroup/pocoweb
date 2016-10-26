#include <chrono>
#include <ctime>
#include <sstream>
#include <regex>
#include <cppconn/connection.h>
#include "Book.hpp"
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
	return session ? 
		Database(session, config_) : 
		boost::optional<Database>{};
}

////////////////////////////////////////////////////////////////////////////////
UserPtr 
Route::cached_find_user(const Database& db, const std::string& name) const
{
	std::lock_guard<std::mutex> lock(db.session().mutex);
	auto get_user_from_db = [&db](const std::string& name) {
		return db.select_user(name);
	};
	return cache_ ? 	
		cache_->user_cache.get(name, get_user_from_db) :
		get_user_from_db(name);
}

////////////////////////////////////////////////////////////////////////////////
UserPtr 
Route::cached_find_user(const Database& db, int userid) const
{
	std::lock_guard<std::mutex> lock(db.session().mutex);
	auto get_user_from_db = [&db](int userid) {
		return db.select_user(userid);
	};
	return cache_ ? 	
		cache_->user_cache.get(userid, get_user_from_db) :
		get_user_from_db(userid);
}

////////////////////////////////////////////////////////////////////////////////
ProjectPtr 
Route::cached_find_book(const Database& db, int projectid) const
{
	std::lock_guard<std::mutex> lock(db.session().mutex);
	auto get_project_from_db = [&db](int projectid) {
		return db.select_project(projectid);
	};
	return cache_ ? 	
		cache_->project_cache.get(projectid, get_project_from_db) :
		get_project_from_db(projectid);
}
