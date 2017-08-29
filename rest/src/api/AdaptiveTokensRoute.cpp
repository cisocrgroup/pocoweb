#include "AdaptiveTokensRoute.hpp"
#include <crow/app.h>
#include <chrono>
#include "core/App.hpp"
#include "core/Book.hpp"
#include "core/Config.hpp"
#include "core/Session.hpp"
#include "core/jsonify.hpp"
#include "core/util.hpp"
#include "database/Tables.h"

#define ADAPTIVE_TOKENS_ROUTE_ROUTE "/books/<int>/adaptive-tokens"

using namespace pcw;

template <class C, class J, class R>
static void append_suggestions(C& conn, J& j, size_t bookid, R& row);
template <class C, class J, class R>
static void lookup_and_append_patterns(C& conn, J& json, size_t bookid,
				       size_t i, const R& row);

////////////////////////////////////////////////////////////////////////////////
const char* AdaptiveTokensRoute::route_ = ADAPTIVE_TOKENS_ROUTE_ROUTE;
const char* AdaptiveTokensRoute::name_ = "AdaptiveTokensRoute";

////////////////////////////////////////////////////////////////////////////////
void AdaptiveTokensRoute::Register(App& app) {
	CROW_ROUTE(app, ADAPTIVE_TOKENS_ROUTE_ROUTE).methods("GET"_method)(*this);
}

SQLPP_ALIAS_PROVIDER(t1_alias);
SQLPP_ALIAS_PROVIDER(t2_alias);
SQLPP_ALIAS_PROVIDER(suggstr);
SQLPP_ALIAS_PROVIDER(tokstr);
// SQLPP_ALIAS_PROVIDER(esugid);

////////////////////////////////////////////////////////////////////////////////
AdaptiveTokensRoute::Response AdaptiveTokensRoute::impl(HttpGet, const Request& req,
						  int bid) const {
	CROW_LOG_DEBUG
	    << "(AdaptiveTokensRoute) lookup adaptive tokens for project id: "
	    << bid;
	LockedSession session(must_find_session(req));
	auto conn = must_get_connection();
	session->assert_permission(conn, bid, Permissions::Read);
	Json j;
	tables::Types t;
	tables::Adaptivetokens a;
	j["adaptiveTokens"] = crow::json::rvalue(crow::json::type::List);
	auto rows = conn.db()(select(t.string).from(t.join(a).on(t.typid == a.typid and t.bookid == a.bookid)).where(t.bookid == bid));
	size_t i = 0;
	for (const auto& row : rows) {
		j["adaptiveTokens"][i++] = row.string;
	}
	return j;
}
