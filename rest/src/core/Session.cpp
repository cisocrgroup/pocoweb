#include <regex>
#include <crow/http_request.h>
#include <crow/http_response.h>
#include <crow/logging.h>
#include "util.hpp"
#include "User.hpp"
#include "Session.hpp"
#include "Cache.hpp"
#include "AppCache.hpp"

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
Session::Session(const User& user, AppCacheSptr cache)
	: sid_(gensessionid(16))
	, user_(user.shared_from_this())
	, cache_(std::move(cache))
	, expiration_date_()
	, mutex_()
	, project_()
	, page_()
{}

////////////////////////////////////////////////////////////////////////////////
bool
Session::has_expired() const noexcept
{
	const auto now = std::chrono::system_clock::now();
	return expiration_date_ <= now;
}

////////////////////////////////////////////////////////////////////////////////
void
Session::cache(Project& view) const
{
	if (cache_) {
		if (view.is_book()) {
			auto book = std::dynamic_pointer_cast<Book>(
					view.shared_from_this());
			cache_->books.put(book);
		} else {
			cache_->projects.put(view.shared_from_this());
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
void
Session::cache(User& user) const
{
	if (cache_) {
		cache_->users.put(user.shared_from_this());
	}
}

////////////////////////////////////////////////////////////////////////////////
void
Session::set_cookies(crow::response& response) const noexcept
{
	set_cookie(response, "pcw-sid", id(), expiration_date_);
	set_cookie(response, "pcw-user", user_->name, expiration_date_);
}

////////////////////////////////////////////////////////////////////////////////
void
pcw::set_cookie(crow::response& response,
		const std::string& key,
		const std::string& val,
		const Session::TimePoint& expires) noexcept
{
	static const std::string SetCookie{"Set-Cookie"};
	const auto e = std::chrono::system_clock::to_time_t(expires);
	std::ostringstream os;
	std::string stime(std::ctime(&e));
	const auto end = stime.find_last_of('\n');
	assert(end != std::string::npos);
	stime = stime.substr(0, end);
	os << key << "=" << val
	   << "; path=/"
	   << "; expires=" << stime
	   << "; domain=pocoweb.cis.lmu.de" << ";";
	response.add_header(SetCookie, os.str());
}

////////////////////////////////////////////////////////////////////////////////
boost::optional<std::string>
pcw::get_cookie(const crow::request& request, const std::string& key) noexcept
{
	static const std::string Cookie{"Cookie"};
	const std::regex re{key + R"(=([a-z]*);?)"};
	const auto range = request.headers.equal_range(Cookie);
	std::smatch m;
	for (auto i = range.first; i != range.second; ++i) {
		CROW_LOG_DEBUG << "(get_cookie) searching: " << i->first << ": " << i->second;
		if (std::regex_match(i->second, m, re))
			return std::string(m[1]);
	}
	return {};
}

