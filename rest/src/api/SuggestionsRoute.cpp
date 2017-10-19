#include "SuggestionsRoute.hpp"
#include <crow/app.h>
#include <chrono>
#include "core/App.hpp"
#include "core/Book.hpp"
#include "core/Config.hpp"
#include "core/Session.hpp"
#include "core/jsonify.hpp"
#include "core/queries.hpp"
#include "core/util.hpp"
#include "database/Tables.h"

#define SUGGESTIONS_ROUTE_ROUTE_1 "/books/<int>/suggestions"
#define SUGGESTIONS_ROUTE_ROUTE_2 \
	"/books/<int>/pages/<int>/lines/<int>/tokens/<int>/suggestions"

using namespace pcw;

template <class C, class J, class R>
static void append_suggestions(C& conn, J& j, size_t bookid, R& row);
template <class C, class J, class R>
static void lookup_and_append_patterns(C& conn, J& json, size_t bookid,
				       size_t i, const R& row);

////////////////////////////////////////////////////////////////////////////////
const char* SuggestionsRoute::route_ =
    SUGGESTIONS_ROUTE_ROUTE_1 "," SUGGESTIONS_ROUTE_ROUTE_2;
const char* SuggestionsRoute::name_ = "SuggestionsRoute";

////////////////////////////////////////////////////////////////////////////////
void SuggestionsRoute::Register(App& app) {
	CROW_ROUTE(app, SUGGESTIONS_ROUTE_ROUTE_1).methods("GET"_method)(*this);
	CROW_ROUTE(app, SUGGESTIONS_ROUTE_ROUTE_2).methods("GET"_method)(*this);
}

SQLPP_ALIAS_PROVIDER(t1_alias);
SQLPP_ALIAS_PROVIDER(t2_alias);
SQLPP_ALIAS_PROVIDER(suggstr);
SQLPP_ALIAS_PROVIDER(tokstr);
// SQLPP_ALIAS_PROVIDER(esugid);

////////////////////////////////////////////////////////////////////////////////
SuggestionsRoute::Response SuggestionsRoute::impl(HttpGet, const Request& req,
						  int bid) const {
	const auto q = query_get<std::string>(req.url_params, "q");
	const auto p = query_get<bool>(req.url_params, "p");
	if (p and *p and not q) {
		THROW(BadRequest, "missing query parameter for pattern search");
	}
	if (not p or p == false) {
		return suggestions(req, bid, q, false);
	} else {
		return suggestions(req, bid, q, true);
	}
}

////////////////////////////////////////////////////////////////////////////////
SuggestionsRoute::Response SuggestionsRoute::suggestions(
    const Request& req, int bid, const boost::optional<std::string>& q,
    bool pat) const {
	CROW_LOG_DEBUG
	    << "(SuggestionsRoute) lookup suggestions for project id: " << bid;
	LockedSession session(must_find_session(req));
	auto conn = must_get_connection();
	session->assert_permission(conn, bid, Permissions::Read);
	const auto project = session->must_find(conn, bid);
	// The project does exist.  If no profile is found, this
	// indicates not a
	// 404 but simply an empty profile.
	using namespace sqlpp;
	tables::Profiles p;
	Json j;
	j["projectId"] = bid;
	j["profiled"] = false;
	j["timestamp"] = 0;
	j["suggestions"] = crow::json::rvalue(crow::json::type::List);
	const auto profile =
	    conn.db()(select(p.timestamp).from(p).where(p.bookid == bid));
	if (profile.empty()) {
		return j;
	}
	j["profiled"] = true;
	j["timestamp"] = profile.front().timestamp;
	if (not pat and q) {
		j["query"] = *q;
		std::string query = *q;
		to_lower(query);
		tables::Types t;
		tables::Suggestions s;
		auto t1 = t.as(t1_alias);
		auto t2 = t.as(t2_alias);
		// lookup typid of query token
		const auto qidrow = conn.db()(select(t.typid).from(t).where(
		    t.bookid == bid and t.string == query));
		if (qidrow.empty()) {
			return j;
		}
		const auto qid = qidrow.front().typid;
		auto rows =
		    conn.db()(select(all_of(s), t1.string.as(tokstr),
				     t2.string.as(suggstr))
				  .from(s.join(t1)
					    .on(t1.typid == s.typid)
					    .join(t2)
					    .on(t2.typid == s.suggestiontypid))
				  .where(s.bookid == bid and s.typid == qid));
		append_suggestions(conn, j, bid, rows);
	} else if (not pat) {
		j["query"] = "";
		tables::Types t;
		tables::Suggestions s;
		auto t1 = t.as(t1_alias);
		auto t2 = t.as(t2_alias);
		auto rows =
		    conn.db()(select(all_of(s), t1.string.as(tokstr),
				     t2.string.as(suggstr))
				  .from(s.join(t1)
					    .on(t1.typid == s.typid)
					    .join(t2)
					    .on(t2.typid == s.suggestiontypid))
				  .where(s.bookid == bid));
		append_suggestions(conn, j, bid, rows);
	} else if (pat and q) {
		j["query"] = *q;
		std::string query = *q;
		to_lower(query);
		tables::Types t;
		tables::Suggestions s;
		tables::Errorpatterns e;
		auto t1 = t.as(t1_alias);
		auto t2 = t.as(t2_alias);
		auto rows = conn.db()(
		    select(all_of(s), t1.string.as(tokstr),
			   t2.string.as(suggstr))
			.from(s.join(t1)
				  .on(t1.typid == s.typid)
				  .join(t2)
				  .on(t2.typid == s.suggestiontypid)
				  .join(e)
				  .on(s.suggestionid == e.suggestionid))
			.where(s.bookid == bid and e.pattern == query));
		append_suggestions(conn, j, bid, rows);
	}
	return j;
}

////////////////////////////////////////////////////////////////////////////////
SuggestionsRoute::Response SuggestionsRoute::impl(HttpGet, const Request& req,
						  int pid, int p, int lid,
						  int tid) const {
	CROW_LOG_DEBUG
	    << "(SuggestionsRoute) lookup suggestions for project id: " << pid;
	LockedSession session(must_find_session(req));
	auto conn = must_get_connection();
	session->assert_permission(conn, pid, Permissions::Read);
	const auto project = session->must_find(conn, pid);
	// The project does exist.  If no profile is found, this
	// indicates not a
	// 404 but simply an empty profile.
	using namespace sqlpp;
	tables::Profiles ptab;
	Json j;
	j["projectId"] = pid;
	j["profiled"] = false;
	j["timestamp"] = 0;
	j["suggestions"] = crow::json::rvalue(crow::json::type::List);
	const auto profile = conn.db()(
	    select(ptab.timestamp).from(ptab).where(ptab.bookid == pid));
	if (profile.empty()) {
		return j;
	}
	j["profiled"] = true;
	j["timestamp"] = profile.front().timestamp;
	tables::Types t;
	tables::Suggestions s;
	auto t1 = t.as(t1_alias);
	auto t2 = t.as(t2_alias);
	auto rows = conn.db()(
	    select(all_of(s), t1.string.as(tokstr), t2.string.as(suggstr))
		.from(s.join(t1)
			  .on(t1.typid == s.typid)
			  .join(t2)
			  .on(t2.typid == s.suggestiontypid))
		.where(s.bookid == pid and s.pageid == p and s.lineid == lid and
		       s.tokenid == tid));
	append_suggestions(conn, j, pid, rows);
	return j;
}

////////////////////////////////////////////////////////////////////////////////
template <class C, class J, class R>
void append_suggestions(C& conn, J& j, size_t bookid, R& rs) {
	size_t i = 0;
	for (const auto& r : rs) {
		j["suggestions"][i]["token"] = r.tokstr;
		j["suggestions"][i]["suggestion"] = r.suggstr;
		j["suggestions"][i]["lineId"] = r.lineid;
		j["suggestions"][i]["pageId"] = r.pageid;
		j["suggestions"][i]["tokenId"] = r.tokenid;
		j["suggestions"][i]["weight"] = r.weight;
		j["suggestions"][i]["distance"] = r.distance;
		lookup_and_append_patterns(conn, j, bookid, i, r);
		i++;
	}
}

////////////////////////////////////////////////////////////////////////////////
template <class C, class J, class R>
void lookup_and_append_patterns(C& conn, J& j, size_t bookid, size_t i,
				const R& row) {
	tables::Errorpatterns e;
	auto rs = conn.db()(select(e.pattern).from(e).where(e.suggestionid ==
							    row.suggestionid));
	j["suggestions"][i]["patterns"] =
	    crow::json::rvalue(crow::json::type::List);
	size_t k = 0;
	for (const auto& r : rs) {
		j["suggestions"][i]["patterns"][k] = r.pattern;
		k++;
	}
}
