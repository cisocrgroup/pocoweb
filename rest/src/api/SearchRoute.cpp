#include "SearchRoute.hpp"
#include <crow.h>
#include <boost/filesystem.hpp>
#include <random>
#include <regex>
#include "core/Archiver.hpp"
#include "core/Book.hpp"
#include "core/BookDirectoryBuilder.hpp"
#include "core/Package.hpp"
#include "core/Page.hpp"
#include "core/Searcher.hpp"
#include "core/Session.hpp"
#include "core/User.hpp"
#include "core/WagnerFischer.hpp"
#include "core/jsonify.hpp"
#include "core/util.hpp"
#include "utils/Error.hpp"
#include "utils/ScopeGuard.hpp"

#define SEARCH_ROUTE_ROUTE "/books/<int>/search"

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
const char* SearchRoute::route_ = SEARCH_ROUTE_ROUTE;
const char* SearchRoute::name_ = "SearchRoute";

////////////////////////////////////////////////////////////////////////////////
void SearchRoute::Register(App& app) {
	CROW_ROUTE(app, SEARCH_ROUTE_ROUTE).methods("GET"_method)(*this);
}

////////////////////////////////////////////////////////////////////////////////
Route::Response SearchRoute::impl(HttpGet, const Request& req, int bid) const {
	return search(req, bid);
}

////////////////////////////////////////////////////////////////////////////////
Route::Response SearchRoute::search(const Request& req, int bid) const {
	const auto q = req.url_params.get("q");
	if (not q or strlen(q) <= 0) {
		THROW(BadRequest, "invalid query string");
	}
	CROW_LOG_DEBUG << "(SearchRoute::search) searching project id: " << bid
		       << " q: " << q;
	const LockedSession session(must_find_session(req));
	auto conn = must_get_connection();
	session->assert_permission(conn, bid, Permissions::Read);
	const auto project = session->must_find(conn, bid);
	Searcher searcher(*project);
	const auto matches = searcher.find(q);
	CROW_LOG_DEBUG << "(SearchRoute::search) found " << matches.size()
		       << " matches for q='" << q << "'";
	Json json;
	size_t i = 0;
	size_t words = 0;
	json["query"] = q;
	json["projectId"] = bid;
	json["nLines"] = matches.size();
	json["nWords"] = words;
	for (const auto& m : matches) {
		json["matches"][i]["line"] << *m.first;
		size_t j = 0;
		for (const auto& token : m.second) {
			json["matches"][i]["matches"][j] << token;
			j++;
			words++;
		}
		++i;
	}
	json["nWords"] = words;
	return json;
}
