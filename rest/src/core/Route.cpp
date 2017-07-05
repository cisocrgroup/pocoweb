#include "Route.hpp"
#include <crow/logging.h>
#include <chrono>
#include <ctime>
#include <regex>
#include <sstream>
#include "AppCache.hpp"
#include "Book.hpp"
#include "Cache.hpp"
#include "Line.hpp"
#include "Page.hpp"
#include "Session.hpp"
#include "SessionStore.hpp"
#include "User.hpp"
#include "utils/Error.hpp"

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
Route::~Route() noexcept {}

////////////////////////////////////////////////////////////////////////////////
SessionPtr Route::new_session(const User& user) const {
	assert(session_store_);
	return session_store_->new_session(user, cache_);
}

////////////////////////////////////////////////////////////////////////////////
SessionPtr Route::find_session(const crow::request& request) const {
	auto sid = get_cookie(request, "pcw-sid");
	CROW_LOG_DEBUG << "(Route) searching for sid: " << sid;
	if (not sid) return nullptr;
	assert(session_store_);
	return session_store_->find_session(*sid);
}

////////////////////////////////////////////////////////////////////////////////
SessionPtr Route::session(const crow::request& request) const {
	auto session = find_session(request);
	if (not session) THROW(Forbidden, "Not logged in");
	return session;
}

////////////////////////////////////////////////////////////////////////////////
void Route::delete_session(const Session& session) const {
	assert(session_store_);
	CROW_LOG_DEBUG << "(Route) deleting session: " << session.id();
	session_store_->delete_session(session.id());
}

////////////////////////////////////////////////////////////////////////////////
std::string Route::extract_content(const crow::request& request) {
	static const std::string ContentType{"Content-Type"};
	static const std::regex BoundaryRegex{R"(boundary=(.*);?$)"};
	// CROW_LOG_INFO << "### BODY ###\n" << request.body;

	std::smatch m;
	if (std::regex_search(request.get_header_value(ContentType), m,
			      BoundaryRegex))
		return extract_multipart(request, m[1]);
	else
		return extract_raw(request);
}

////////////////////////////////////////////////////////////////////////////////
std::string Route::extract_multipart(const crow::request& request,
				     const std::string& boundary) {
	CROW_LOG_INFO << "(Route) Boundary in multipart data: " << boundary;
	auto b = request.body.find("\r\n\r\n");
	if (b == std::string::npos)
		THROW(BadRequest,
		      "(Route) Missing '\\r\\n\\r\\n' in multipart data");
	b += 4;
	auto e = request.body.rfind(boundary);
	if (b == std::string::npos)
		THROW(BadRequest, "(Route) Boundary in multipart data ",
		      boundary);
	if (e < b)
		THROW(BadRequest, "(Route) Boundary in multipart data ",
		      boundary);
	CROW_LOG_INFO << "(Route) b = " << b << ", e = " << e;

	return request.body.substr(b, e - b);
}

////////////////////////////////////////////////////////////////////////////////
std::string Route::extract_raw(const crow::request& request) {
	return request.body;
}
