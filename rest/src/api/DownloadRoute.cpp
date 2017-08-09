#include "DownloadRoute.hpp"
#include <crow.h>
#include <boost/filesystem.hpp>
#include <random>
#include <regex>
#include "core/Archiver.hpp"
#include "core/Book.hpp"
#include "core/Package.hpp"
#include "core/Page.hpp"
#include "core/Searcher.hpp"
#include "core/Session.hpp"
#include "core/User.hpp"
#include "core/jsonify.hpp"
#include "core/util.hpp"
#include "utils/Error.hpp"

#define DOWNLOAD_ROUTE_ROUTE "/books/<int>/download"

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
const char* DownloadRoute::route_ = DOWNLOAD_ROUTE_ROUTE;
const char* DownloadRoute::name_ = "DownloadRoute";

////////////////////////////////////////////////////////////////////////////////
void DownloadRoute::Register(App& app) {
	CROW_ROUTE(app, DOWNLOAD_ROUTE_ROUTE).methods("GET"_method)(*this);
}

////////////////////////////////////////////////////////////////////////////////
Route::Response DownloadRoute::impl(HttpGet, const Request& req,
				    int bid) const {
	return download(req, bid);
}

////////////////////////////////////////////////////////////////////////////////
Route::Response DownloadRoute::download(const Request& req, int bid) const {
	CROW_LOG_DEBUG << "(DownloadRoute::download) downloading project id: "
		       << bid;
	auto conn = must_get_connection();
	const LockedSession session(must_find_session(req));
	session->assert_permission(conn, bid, Permissions::Read);
	const auto project = session->must_find(conn, bid);
	Archiver archiver(*project);
	auto ar = archiver();
	Json j;
	j["archive"] = ar.string();
	return j;
}
