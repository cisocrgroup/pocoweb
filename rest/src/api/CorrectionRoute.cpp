#include "CorrectionRoute.hpp"
#include <crow.h>
#include <crow/json.h>
#include <database/Database.hpp>
#include "core/Line.hpp"
#include "core/Session.hpp"
#include "core/WagnerFischer.hpp"
#include "core/jsonify.hpp"

#define CORRECTION_ROUTE_ROUTE "/correction"

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
const char* CorrectionRoute::route_ = CORRECTION_ROUTE_ROUTE;
const char* CorrectionRoute::name_ = "CorrectionRoute";

////////////////////////////////////////////////////////////////////////////////
void CorrectionRoute::Register(App& app) {
	CROW_ROUTE(app, CORRECTION_ROUTE_ROUTE).methods("POST"_method)(*this);
}

////////////////////////////////////////////////////////////////////////////////
Route::Response CorrectionRoute::impl(HttpPost, const Request& req) const {
	const auto json = crow::json::load(req.body);
	const auto pid = get<int>(json, "projectId");
	if (not pid)
		THROW(BadRequest,
		      "(CorrectionRoute) missing projectId in POST data");
	const auto p = get<int>(json, "pageId");
	if (not p)
		THROW(BadRequest,
		      "(CorrectionRoute) missing pageId in POST data");
	const auto lid = get<int>(json, "lineId");
	if (not lid)
		THROW(BadRequest,
		      "(CorrectionRoute) missing lineId in POST data");
	const auto c = get<std::string>(json, "correction");
	if (not c)
		THROW(BadRequest,
		      "(CorrectionRoute) missing correction in POST data");
	const auto tid = get<int>(json, "tokenId");
	auto conn = connection();
	const auto session = this->session(req);
	assert(conn);
	assert(session);
	session->has_permission_or_throw(conn, *pid, Permissions::Write);
	const auto line = session->find(conn, *pid, *p, *lid);
	if (not line) THROW(NotFound, "could not find requested line");

	if (tid) {
		return impl(conn, *line, *tid, *c);
	} else {
		return impl(conn, *line, *c);
	}
}

////////////////////////////////////////////////////////////////////////////////
Route::Response CorrectionRoute::impl(MysqlConnection& conn, Line& line,
				      const std::string& c) const {
	WagnerFischer wf;
	wf.set_gt(c);
	wf.set_ocr(line);
	const auto lev = wf();
	CROW_LOG_DEBUG << "(CorrectionRoute) correction: " << c;
	CROW_LOG_DEBUG << "(CorrectionRoute) line.cor(): " << line.cor();
	CROW_LOG_DEBUG << "(CorrectionRoute) line.ocr(): " << line.ocr();
	CROW_LOG_DEBUG << "(CorrectionRoute)        lev: " << lev;
	wf.correct(line);
	update_line(conn, line);
	Json j;
	return j << line;
}

////////////////////////////////////////////////////////////////////////////////
Route::Response CorrectionRoute::impl(MysqlConnection& conn, Line& line,
				      int tid, const std::string& c) const {
	return internal_server_error();
}

////////////////////////////////////////////////////////////////////////////////
void CorrectionRoute::update_line(MysqlConnection& conn, const Line& line) {
	MysqlCommiter commiter(conn);
	pcw::update_line(conn.db(), line);
	commiter.commit();
}
