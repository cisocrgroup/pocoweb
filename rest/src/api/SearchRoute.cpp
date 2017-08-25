#include "SearchRoute.hpp"
#include <crow.h>
#include <boost/filesystem.hpp>
#include <random>
#include <regex>
#include <set>
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

template <class M>
static SearchRoute::Response make_response(const M& matches, int bookid,
					   const std::string& q, bool ep);

////////////////////////////////////////////////////////////////////////////////
const char* SearchRoute::route_ = SEARCH_ROUTE_ROUTE;
const char* SearchRoute::name_ = "SearchRoute";

////////////////////////////////////////////////////////////////////////////////
void SearchRoute::Register(App& app) {
	CROW_ROUTE(app, SEARCH_ROUTE_ROUTE).methods("GET"_method)(*this);
}

////////////////////////////////////////////////////////////////////////////////
Route::Response SearchRoute::impl(HttpGet, const Request& req, int bid) const {
	const auto q = get_query(req);
	if (is_error_pattern_query(req)) {
		return search(req, q, bid, ErrorPatternQuery{});
	} else {
		return search(req, q, bid, TokenQuery{});
	}
}

////////////////////////////////////////////////////////////////////////////////
Route::Response SearchRoute::search(const Request& req, const std::string& q,
				    int bid, TokenQuery) const {
	CROW_LOG_DEBUG << "(SearchRoute::search) searching project id: " << bid
		       << " query string q: " << q;
	const LockedSession session(must_find_session(req));
	auto conn = must_get_connection();
	session->assert_permission(conn, bid, Permissions::Read);
	const auto project = session->must_find(conn, bid);
	Searcher searcher(*project);
	const auto matches = searcher.find(q);
	CROW_LOG_DEBUG << "(SearchRoute::search) found " << matches.size()
		       << " matches for q='" << q << "'";
	return make_response(matches, bid, q, false);
}

////////////////////////////////////////////////////////////////////////////////
Route::Response SearchRoute::search(const Request& req, const std::string& q,
				    int bid, ErrorPatternQuery) const {
	CROW_LOG_DEBUG << "(SearchRoute::search) searching project id: " << bid
		       << " for error pattern q: " << q;
	const LockedSession session(must_find_session(req));
	auto conn = must_get_connection();
	session->assert_permission(conn, bid, Permissions::Read);
	const auto project = session->must_find(conn, bid);
	tables::Errorpatterns e;
	tables::Suggestions s;
	auto rows =
	    conn.db()(select(s.pageid, s.lineid, s.tokenid)
			  .from(s.join(e).on(s.suggestionid == e.suggestionid))
			  .where(e.bookid == bid and e.pattern == q));
	std::set<std::tuple<int, int, int>> ids;
	for (const auto& row : rows) {
		ids.emplace(row.pageid, row.lineid, row.tokenid);
	}
	Searcher searcher(*project);
	const auto matches = searcher.find_impl([&ids](const auto& t) {
		auto p =
		    std::make_tuple(t.line->page().id(), t.line->id(), t.id);
		return ids.count(p);
	});
	CROW_LOG_DEBUG << "(SearchRoute::search) found " << matches.size()
		       << " matches for q='" << q << "'";
	return make_response(matches, bid, q, true);
}

////////////////////////////////////////////////////////////////////////////////
std::string SearchRoute::get_query(const Request& req) {
	const auto q = req.url_params.get("q");
	if (not q or strlen(q) <= 0) {
		THROW(BadRequest,
		      "(SearchRoute) invalid or missing query string");
	}
	return q;
}

////////////////////////////////////////////////////////////////////////////////
bool SearchRoute::is_error_pattern_query(const Request& req) {
	const auto p = req.url_params.get("p");
	if (not p) {
		return false;
	}
	try {
		const auto i = std::stoi(p);
		return i;
	} catch (const std::invalid_argument&) {
	} catch (const std::out_of_range&) {
	}
	THROW(BadRequest, "(SearchRoute) invalid parameter p: ", p);
}

////////////////////////////////////////////////////////////////////////////////
template <class M>
SearchRoute::Response make_response(const M& matches, int bookid,
				    const std::string& q, bool ep) {
	Json json;
	size_t i = 0;
	size_t words = 0;
	json["query"] = q;
	json["isErrorPattern"] = ep;
	json["projectId"] = bookid;
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
