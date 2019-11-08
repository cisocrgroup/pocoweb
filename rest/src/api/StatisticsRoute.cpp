#include "StatisticsRoute.hpp"
#include "database/DbStructs.hpp"
#include <crow.h>

#define STATISTICS_ROUTE_ROUTE_1 "/books/<int>/statistics"
using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
const char *StatisticsRoute::route_ = STATISTICS_ROUTE_ROUTE_1;
const char *StatisticsRoute::name_ = "StatisticsRoute";

////////////////////////////////////////////////////////////////////////////////
void StatisticsRoute::Register(App &app) {
  CROW_ROUTE(app, STATISTICS_ROUTE_ROUTE_1).methods("GET"_method)(*this);
}

////////////////////////////////////////////////////////////////////////////////
// GET /books/bid/statistics
////////////////////////////////////////////////////////////////////////////////
Route::Response StatisticsRoute::impl(HttpGet, const Request &req,
                                      int bid) const {
  CROW_LOG_INFO << "(StatisticsRoute) GET statistics: " << bid;
  auto conn = must_get_connection();
  DbPackage pkg(bid);
  if (not pkg.load(conn)) {
    THROW(NotFound, "cannot find package id: ", bid);
  }
  Statistics stats{}; // zero initialize
  if (not stats.load(conn, pkg)) {
    THROW(NotFound, "cannot load statistics for package: ", bid);
  }
  rapidjson::StringBuffer buf;
  rapidjson::Writer<rapidjson::StringBuffer> writer(buf);
  stats.serialize(writer);
  Response res(200, std::string(buf.GetString(), buf.GetSize()));
  res.set_header("Content-Type", "application/json");
  return res;
}
