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
static void make_response(Json &json, const M &matches, int bookid,
                          const std::string &q, bool ep);

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
  Json json;
  json["projectId"] = bid;
  json["isErrorPattern"] = false;
  for (const auto &q : qs) {
    CROW_LOG_DEBUG << "(SearchRoute::search) searching project id: " << bid
                   << " for query string q: " << q;

    auto conn = must_get_connection();
    const auto project = session->must_find(conn, bid);
    Searcher searcher(*project);
    const auto matches = searcher.find(q);
    CROW_LOG_DEBUG << "(SearchRoute::search) found " << matches.size()
                   << " matches for q='" << q << "'";
    make_response(json, matches, bid, q, false);
  }
  return json;
}

////////////////////////////////////////////////////////////////////////////////
Route::Response SearchRoute::search(const Request &req,
                                    const std::vector<std::string> &qs, int bid,
                                    ErrorPatternQuery) const {
  CROW_LOG_DEBUG << "(SearchRoute::search) searching project id: " << bid
                 << " for error pattern q: " << qs[0];
  // const LockedSession session(get_session(req));
  // auto conn = must_get_connection();
  // const auto project = session->must_find(conn, bid);
  // tables::Errorpatterns e;
  // tables::Suggestions s;
  // auto rows =
  //     conn.db()(select(s.pageid, s.lineid, s.tokenid)
  //                   .from(s.join(e).on(s.suggestiontypid ==
  //                   e.suggestiontypid)) .where(e.bookid == bid and e.pattern
  //                   == q));
  // std::set<std::tuple<int, int, int>> ids;
  // for (const auto &row : rows) {
  //   ids.emplace(row.pageid, row.lineid, row.tokenid);
  // }
  // Searcher searcher(*project);
  // const auto matches = searcher.find_impl([&ids](const auto &t) {
  //   auto p = std::make_tuple(t.line->page().id(), t.line->id(), t.id);
  //   return ids.count(p);
  // });
  // CROW_LOG_DEBUG << "(SearchRoute::search) found " << matches.size()
  //                << " matches for q='" << q << "'";
  // return make_response(matches, bid, q, true);
  THROW(BadRequest, "(SearchRoute) pattern search currently not implemented");
}

////////////////////////////////////////////////////////////////////////////////
template <class M>
void make_response(Json &json, const M &matches, int bookid,
                   const std::string &q, bool ep) {
  CROW_LOG_DEBUG << "(SearchRoute::search) building response";
  size_t i = 0;
  for (const auto &m : matches) {
    json["matches"][i]["line"] << *m.first;
    size_t j = 0;
    for (const auto &token : m.second) {
      json["matches"][i]["tokens"][j++] << token;
    }
    ++i;
  }
  CROW_LOG_DEBUG << "(SearchRoute::search) built response";
}
