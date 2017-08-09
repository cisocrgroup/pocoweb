#include "ErrorPatternsRoute.hpp"
#include <crow/app.h>
#include "core/App.hpp"
#include "core/Book.hpp"
#include "core/Config.hpp"
#include "core/Session.hpp"
#include "core/jsonify.hpp"
#include "core/util.hpp"

#define PROFILER_ROUTE_ROUTE "/books/<int>/error-patterns"

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
const char* ErrorPatternsRoute::route_ = PROFILER_ROUTE_ROUTE;
const char* ErrorPatternsRoute::name_ = "ErrorPatternsRoute";

////////////////////////////////////////////////////////////////////////////////
void ErrorPatternsRoute::Register(App& app) {
	CROW_ROUTE(app, PROFILER_ROUTE_ROUTE).methods("GET"_method)(*this);
}

SQLPP_ALIAS_PROVIDER(t1_alias);
SQLPP_ALIAS_PROVIDER(t2_alias);
SQLPP_ALIAS_PROVIDER(suggstr);
SQLPP_ALIAS_PROVIDER(tokstr);
////////////////////////////////////////////////////////////////////////////////
ErrorPatternsRoute::Response ErrorPatternsRoute::impl(HttpGet,
						      const Request& req,
						      int bid) const {
	CROW_LOG_DEBUG << "(ErrorPatternsRoute) lookup profile for project id: "
		       << bid;
	LockedSession session(must_find_session(req));
	auto conn = must_get_connection();
	session->assert_permission(conn, bid, Permissions::Read);
	const auto project = session->must_find(conn, bid);
	const auto q = req.url_params.get("q");
	// The project does exist.  If no profile is found, this indicates not a
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
	if (q and strlen(q) > 0) {
		j["query"] = q;
		std::string query = q;
		to_lower(query);
		tables::Types t;
		tables::Suggestions s;
		tables::Errorpatterns e;
		auto t1 = t.as(t1_alias);
		auto t2 = t.as(t2_alias);
		auto rows = conn.db()(
		    select(all_of(s), e.pattern, t1.string.as(tokstr),
			   t2.string.as(suggstr))
			.from(e.join(s)
				  .on(e.bookid == s.bookid and
				      e.pageid == s.pageid and
				      e.lineid == s.lineid and
				      e.tokenid == s.tokenid)
				  .join(t1)
				  .on(t1.typid == s.typid)
				  .join(t2)
				  .on(t2.typid == s.suggestionid))
			.where(e.pattern == query and s.bookid == bid));
		size_t i = 0;
		for (const auto& row : rows) {
			j["suggestions"][i]["lineId"] = row.lineid;
			j["suggestions"][i]["pageId"] = row.pageid;
			j["suggestions"][i]["tokenId"] = row.tokenid;
			j["suggestions"][i]["token"] = row.tokstr;
			j["suggestions"][i]["suggestion"] = row.suggstr;
			j["suggestions"][i]["weight"] = row.weight;
			j["suggestions"][i]["distance"] = row.distance;
			j["suggestions"][i]["pattern"] = row.pattern;
			i++;
		}
	} else {
		tables::Types t;
		tables::Suggestions s;
		tables::Errorpatterns e;
		auto t1 = t.as(t1_alias);
		auto t2 = t.as(t2_alias);
		auto rows =
		    conn.db()(select(all_of(s), e.pattern, t1.string.as(tokstr),
				     t2.string.as(suggstr))
				  .from(e.join(s)
					    .on(e.bookid == s.bookid and
						e.pageid == s.pageid and
						e.lineid == s.lineid and
						e.tokenid == s.tokenid)
					    .join(t1)
					    .on(t1.typid == s.typid)
					    .join(t2)
					    .on(t2.typid == s.suggestionid))
				  .where(s.bookid == bid));
		size_t i = 0;
		for (const auto& row : rows) {
			j["suggestions"][i]["lineId"] = row.lineid;
			j["suggestions"][i]["pageId"] = row.pageid;
			j["suggestions"][i]["tokenId"] = row.tokenid;
			j["suggestions"][i]["token"] = row.tokstr;
			j["suggestions"][i]["suggestion"] = row.suggstr;
			j["suggestions"][i]["weight"] = row.weight;
			j["suggestions"][i]["distance"] = row.distance;
			j["suggestions"][i]["pattern"] = row.pattern;
			i++;
		}
	}
	return j;
}
