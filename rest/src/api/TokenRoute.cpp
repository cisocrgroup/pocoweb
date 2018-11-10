#include "TokenRoute.hpp"
#include "core/Book.hpp"
#include "core/BookDirectoryBuilder.hpp"
#include "core/Package.hpp"
#include "core/Page.hpp"
#include "core/Session.hpp"
#include "core/SessionDirectory.hpp"
#include "core/WagnerFischer.hpp"
#include "core/jsonify.hpp"
#include "utils/Error.hpp"
#include "utils/ScopeGuard.hpp"
#include <crow.h>
#include <regex>
#include <unicode/uchar.h>
#include <utf8.h>

#define TOKEN_ROUTE_ROUTE_1 "/books/<int>/pages/<int>/tokens"
#define TOKEN_ROUTE_ROUTE_2 "/books/<int>/pages/<int>/lines/<int>/tokens"

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
const char* TokenRoute::route_ = TOKEN_ROUTE_ROUTE_1 "," TOKEN_ROUTE_ROUTE_2;
const char* TokenRoute::name_ = "TokenRoute";

////////////////////////////////////////////////////////////////////////////////
void
TokenRoute::Register(App& app)
{
  CROW_ROUTE(app, TOKEN_ROUTE_ROUTE_1).methods("GET"_method)(*this);
  CROW_ROUTE(app, TOKEN_ROUTE_ROUTE_2).methods("GET"_method)(*this);
}

////////////////////////////////////////////////////////////////////////////////
Route::Response
TokenRoute::impl(HttpGet, const Request& req, int bid, int pid) const
{
  LockedSession session(must_find_session(req));
  auto conn = must_get_connection();
  auto page = session->must_find(conn, bid, pid);
  std::vector<Token> tokens;
  for (const auto& line : *page) {
    if (line) {
      for (const auto& word : line->words()) {
        tokens.push_back(word);
      }
    }
  }
  Json j;
  return j << tokens;
}

////////////////////////////////////////////////////////////////////////////////
Route::Response
TokenRoute::impl(HttpGet, const Request& req, int bid, int pid, int lid) const
{
  LockedSession session(must_find_session(req));
  auto conn = must_get_connection();
  auto line = session->must_find(conn, bid, pid, lid);
  Json j;
  return j << line->words();
}
