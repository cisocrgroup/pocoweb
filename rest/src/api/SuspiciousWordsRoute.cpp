#include "SuspiciousWordsRoute.hpp"
#include "core/App.hpp"
#include "core/Book.hpp"
#include "core/Config.hpp"
#include "core/Session.hpp"
#include "core/jsonify.hpp"
#include "core/util.hpp"
#include "database/Tables.h"
#include <chrono>
#include <crow/app.h>

#define SUSPICIOUS_WORDS_ROUTE_ROUTE_1 "/books/<int>/suspicious-words"
#define SUSPICIOUS_WORDS_ROUTE_ROUTE_2                                         \
  "/books/<int>/pages/<int>/suspicious-words"
#define SUSPICIOUS_WORDS_ROUTE_ROUTE_3                                         \
  "/books/<int>/pages/<int>/lines/<int>/suspicious-words"

using namespace pcw;

template<class R>
static SuspiciousWordsRoute::Response
make_response(R& rows, int pid);

////////////////////////////////////////////////////////////////////////////////
const char* SuspiciousWordsRoute::route_ = SUSPICIOUS_WORDS_ROUTE_ROUTE_1
  "," SUSPICIOUS_WORDS_ROUTE_ROUTE_2 "," SUSPICIOUS_WORDS_ROUTE_ROUTE_3;
const char* SuspiciousWordsRoute::name_ = "SuspiciousWordsRoute";

////////////////////////////////////////////////////////////////////////////////
void
SuspiciousWordsRoute::Register(App& app)
{
  CROW_ROUTE(app, SUSPICIOUS_WORDS_ROUTE_ROUTE_1).methods("GET"_method)(*this);
  CROW_ROUTE(app, SUSPICIOUS_WORDS_ROUTE_ROUTE_2).methods("GET"_method)(*this);
  CROW_ROUTE(app, SUSPICIOUS_WORDS_ROUTE_ROUTE_3).methods("GET"_method)(*this);
}

////////////////////////////////////////////////////////////////////////////////
SuspiciousWordsRoute::Response
SuspiciousWordsRoute::impl(HttpGet, const Request& req, int bid) const
{
  CROW_LOG_DEBUG
    << "(SuspiciousWordsRoute) lookup suspicious words for project id: " << bid;
  LockedSession session(get_session(req));
  auto conn = must_get_connection();
  const auto project = session->must_find(conn, bid);
  tables::Suggestions s;
  tables::Types t;
  auto rows = conn.db()(select(t.string, s.pageid, s.lineid, s.tokenid)
                          .from(s.join(t).on(s.typid == t.typid))
                          .where(s.topsuggestion == true and s.bookid == bid));
  return make_response(rows, project->id());
}

////////////////////////////////////////////////////////////////////////////////
SuspiciousWordsRoute::Response
SuspiciousWordsRoute::impl(HttpGet, const Request& req, int bid, int pid) const
{
  CROW_LOG_DEBUG
    << "(SuspiciousWordsRoute) lookup suspicious words for project id: " << bid;
  LockedSession session(get_session(req));
  auto conn = must_get_connection();
  const auto project = session->must_find(conn, bid);
  tables::Suggestions s;
  tables::Types t;
  auto rows = conn.db()(
    select(t.string, s.pageid, s.lineid, s.tokenid)
      .from(s.join(t).on(s.typid == t.typid))
      .where(s.topsuggestion == true and s.bookid == bid and s.pageid == pid));
  return make_response(rows, project->id());
}

////////////////////////////////////////////////////////////////////////////////
SuspiciousWordsRoute::Response
SuspiciousWordsRoute::impl(HttpGet,
                           const Request& req,
                           int bid,
                           int pid,
                           int lid) const
{
  CROW_LOG_DEBUG
    << "(SuspiciousWordsRoute) lookup suspicious words for project id: " << bid;
  LockedSession session(get_session(req));
  auto conn = must_get_connection();
  const auto project = session->must_find(conn, bid);
  tables::Suggestions s;
  tables::Types t;
  auto rows = conn.db()(select(t.string, s.pageid, s.lineid, s.tokenid)
                          .from(s.join(t).on(t.typid == s.typid))
                          .where(s.topsuggestion == true and s.bookid == bid and
                                 s.pageid == pid and s.lineid == lid));
  return make_response(rows, project->id());
}

////////////////////////////////////////////////////////////////////////////////
template<class R>
SuspiciousWordsRoute::Response
make_response(R& rows, int pid)
{
  Json json;
  size_t i = 0;
  json["projectId"] = pid;
  json["suspiciousWords"] = crow::json::rvalue(crow::json::type::List);
  for (const auto& row : rows) {
    json["suspiciousWords"][i]["lineId"] = row.lineid;
    json["suspiciousWords"][i]["pageId"] = row.pageid;
    json["suspiciousWords"][i]["tokenId"] = row.tokenid;
    json["suspiciousWords"][i]["token"] = row.string;
    ++i;
  }
  return json;
}
