#include "LineRoute.hpp"
#include <crow.h>
#include <unicode/uchar.h>
#include <utf8.h>
#include <regex>
#include "core/Book.hpp"
#include "core/BookDirectoryBuilder.hpp"
#include "core/Package.hpp"
#include "core/Page.hpp"
#include "core/Session.hpp"
#include "core/SessionDirectory.hpp"
#include "core/User.hpp"
#include "core/WagnerFischer.hpp"
#include "core/jsonify.hpp"
#include "utils/Error.hpp"
#include "utils/ScopeGuard.hpp"

#define LINE_ROUTE_ROUTE_1 "/books/<int>/pages/<int>/lines/<int>"
#define LINE_ROUTE_ROUTE_2 "/books/<int>/pages/<int>/lines/<int>/<string>"

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
const char* LineRoute::route_ = LINE_ROUTE_ROUTE_2 "," LINE_ROUTE_ROUTE_2;
const char* LineRoute::name_ = "LineRoute";

////////////////////////////////////////////////////////////////////////////////
void LineRoute::Register(App& app) {
	CROW_ROUTE(app, LINE_ROUTE_ROUTE_1).methods("GET"_method)(*this);
	CROW_ROUTE(app, LINE_ROUTE_ROUTE_2).methods("POST"_method)(*this);
}

////////////////////////////////////////////////////////////////////////////////
Route::Response LineRoute::impl(HttpGet, const Request& req, int bid, int pid,
				int lid) const {
	auto conn = connection();
	auto session = this->session(req);
	assert(conn);
	assert(session);
	SessionLock lock(*session);

	auto line = session->find(conn, bid, pid, lid);
	if (not line) return not_found();
	assert(line);
	Json j;
	return j << *line;
}

////////////////////////////////////////////////////////////////////////////////
Route::Response LineRoute::impl(HttpPost, const Request& req, int bid, int pid,
				int lid, const std::string& str) const {
	if (str != "create-split-images") return not_found();
	const auto json = crow::json::load(req.body);
	if (json["projectId"].i() != bid or json["pageId"].i() != pid or
	    json["lineId"].i() != lid)
		THROW(BadRequest,
		      "(LineRoute) post ids do not match request uri");
	auto conn = connection();
	const auto session = this->session(req);
	assert(conn);
	assert(session);
	SessionLock lock(*session);
	session->assert_permission(conn, bid, Permissions::Read);
	const auto line = session->find(conn, bid, pid, lid);
	if (not line) THROW(NotFound, "(LineRoute) cannot find line");
	return create_split_images(*session, *line, json["tokenId"].i());
}

////////////////////////////////////////////////////////////////////////////////
Route::Response LineRoute::create_split_images(const Session& session,
					       const Line& line,
					       int tokenid) const {
	boost::optional<Token> token;
	line.each_token([&](const auto& t) {
		if (t.id == tokenid) token = t;

	});
	if (not token) THROW(NotFound, "(LineRoute) cannot find token");
	const auto split_images =
	    session.directory().create_split_images(line, token->box);
	Json json;
	json["leftImg"] = nullptr;
	json["middleImg"] = nullptr;
	json["rightImg"] = nullptr;
	if (std::get<0>(split_images))
		json["leftImg"] = std::get<0>(split_images)->string();
	if (std::get<1>(split_images))
		json["middleImg"] = std::get<1>(split_images)->string();
	if (std::get<2>(split_images))
		json["rightImg"] = std::get<2>(split_images)->string();
	return json;
}
