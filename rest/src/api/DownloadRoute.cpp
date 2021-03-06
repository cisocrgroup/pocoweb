#include "DownloadRoute.hpp"
#include "core/Archiver.hpp"
#include "core/jsonify.hpp"
#include <crow.h>

#define DOWNLOAD_ROUTE_ROUTE "/books/<int>/download"

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
const char *DownloadRoute::route_ = DOWNLOAD_ROUTE_ROUTE;
const char *DownloadRoute::name_ = "DownloadRoute";

////////////////////////////////////////////////////////////////////////////////
void DownloadRoute::Register(App &app) {
  CROW_ROUTE(app, DOWNLOAD_ROUTE_ROUTE).methods("GET"_method)(*this);
}

////////////////////////////////////////////////////////////////////////////////
Route::Response DownloadRoute::impl(HttpGet, const Request &req,
                                    int bid) const {
  return download(req, bid);
}

////////////////////////////////////////////////////////////////////////////////
Route::Response DownloadRoute::download(const Request &req, int bid) const {
  CROW_LOG_DEBUG << "(DownloadRoute::download) downloading project id: " << bid;
  auto conn = must_get_connection();
  Archiver archiver(bid, conn, get_config());
  auto ar = archiver();
  Json j;
  j["archive"] = ar.string();
  return j;
}
