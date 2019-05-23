#include "SearchRoute.hpp"
#include "core/Archiver.hpp"
#include "core/Book.hpp"
#include "core/BookDirectoryBuilder.hpp"
#include "core/Package.hpp"
#include "core/Page.hpp"
#include "core/Searcher.hpp"
#include "core/Session.hpp"
#include "core/WagnerFischer.hpp"
#include "core/jsonify.hpp"
#include "core/queries.hpp"
#include "core/util.hpp"
#include "utils/Error.hpp"
#include "utils/ScopeGuard.hpp"
#include <boost/filesystem.hpp>
#include <crow.h>
#include <random>
#include <regex>
#include <set>

#define SEARCH_ROUTE_ROUTE "/books/<int>/search"

using namespace pcw;

template <class M>
static void add_matches(Json &json, const M &matches, const std::string &q,
                        bool ep);

////////////////////////////////////////////////////////////////////////////////
const char *SearchRoute::route_ = SEARCH_ROUTE_ROUTE;
const char *SearchRoute::name_ = "SearchRoute";

////////////////////////////////////////////////////////////////////////////////
void SearchRoute::Register(App &app) {
  CROW_ROUTE(app, SEARCH_ROUTE_ROUTE).methods("GET"_method)(*this);
}

////////////////////////////////////////////////////////////////////////////////
Route::Response SearchRoute::impl(HttpGet, const Request &req, int bid) const {
  const auto qs = query_get<std::vector<std::string>>(req.url_params, "q");
  if (not qs) {
    THROW(BadRequest, "(SearchRoute) invalid or missing query parameters");
  }
  const auto p = query_get<bool>(req.url_params, "p");
  if (not p or p == false) {
    return search(req, *qs, bid, TokenQuery{});
  } else {
    return search(req, *qs, bid, ErrorPatternQuery{});
  }
}

////////////////////////////////////////////////////////////////////////////////
Route::Response SearchRoute::search(const Request &req,
                                    const std::vector<std::string> &qs, int bid,
                                    TokenQuery) const {
  const LockedSession session(get_session(req));
  auto conn = must_get_connection();
  const auto project = session->must_find(conn, bid);
  Json json;
  json["matches"] = crow::json::rvalue(crow::json::type::Object);
  json["projectId"] = bid;
  json["isErrorPattern"] = false;
  for (const auto &q : qs) {
    CROW_LOG_DEBUG << "(SearchRoute::search) searching project id: " << bid
                   << " for query string q: " << q;
    Searcher searcher(*project);
    const auto matches = searcher.find(q);
    CROW_LOG_DEBUG << "(SearchRoute::search) found " << matches.size()
                   << " matches for q='" << q << "'";
    add_matches(json, matches, q, false);
  }
  return json;
}

////////////////////////////////////////////////////////////////////////////////
Route::Response SearchRoute::search(const Request &req,
                                    const std::vector<std::string> &qs, int bid,
                                    ErrorPatternQuery) const {
  const LockedSession session(get_session(req));
  auto conn = must_get_connection();
  const auto project = session->must_find(conn, bid);
  std::vector<std::string> tokens;
  for (const auto &q : qs) {
    CROW_LOG_DEBUG << "(SearchRoute::search) searching project id: " << bid
                   << " for error pattern: " << q;
    tables::Errorpatterns e;
    tables::Suggestions s;
    tables::Types t;
    auto rows = conn.db()(select(t.typ)
                              .from(e.join(s)
                                        .on(e.suggestionid == s.id)
                                        .join(t)
                                        .on(t.id == s.tokentypid))
                              .where(e.pattern == q));
    for (const auto &row : rows) {
      tokens.push_back(row.typ);
    }
  }
  Json json;
  json["matches"] = crow::json::rvalue(crow::json::type::Object);
  json["projectId"] = bid;
  json["isErrorPattern"] = false;
  for (const auto t : tokens) {
    Searcher searcher(*project);
    const auto matches = searcher.find(t);
    CROW_LOG_DEBUG << "(SearchRoute::search) found " << matches.size()
                   << " matches for q='" << t << "'";
    add_matches(json, matches, t, true);
  }
  return json;
}

////////////////////////////////////////////////////////////////////////////////
template <class M>
void add_matches(Json &json, const M &matches, const std::string &q, bool ep) {
  CROW_LOG_DEBUG << "(SearchRoute::search) building response";
  size_t i = 0;
  for (const auto &m : matches) {
    json["matches"][q][i]["line"] << *m.first;
    size_t j = 0;
    for (const auto &token : m.second) {
      json["matches"][q][i]["tokens"][j++] << token;
    }
    ++i;
  }
  CROW_LOG_DEBUG << "(SearchRoute::search) built response";
}
