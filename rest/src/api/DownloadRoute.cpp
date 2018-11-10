#include "DownloadRoute.hpp"
#include "core/Archiver.hpp"
#include "core/Book.hpp"
#include "core/Package.hpp"
#include "core/Page.hpp"
#include "core/Searcher.hpp"
#include "core/Session.hpp"
#include "core/jsonify.hpp"
#include "core/util.hpp"
#include "utils/Error.hpp"
#include <boost/filesystem.hpp>
#include <crow.h>
#include <random>
#include <regex>

#define DOWNLOAD_ROUTE_ROUTE "/books/<int>/download"

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
const char* DownloadRoute::route_ = DOWNLOAD_ROUTE_ROUTE;
const char* DownloadRoute::name_ = "DownloadRoute";

////////////////////////////////////////////////////////////////////////////////
void
DownloadRoute::Register(App& app)
{
  CROW_ROUTE(app, DOWNLOAD_ROUTE_ROUTE).methods("GET"_method)(*this);
}

////////////////////////////////////////////////////////////////////////////////
Route::Response
DownloadRoute::impl(HttpGet, const Request& req, int bid) const
{
  return download(req, bid);
}

////////////////////////////////////////////////////////////////////////////////
Route::Response
DownloadRoute::download(const Request& req, int bid) const
{
  CROW_LOG_DEBUG << "(DownloadRoute::download) downloading project id: " << bid;
  auto conn = must_get_connection();
  const LockedSession session(get_session(req));
  const auto project = session->must_find(conn, bid);
  Archiver archiver(*project, conn, get_config());
  auto ar = archiver();
  Json j;
  j["archive"] = ar.string();
  return j;
}
