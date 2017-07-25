#include "CorrectionRoute.hpp"
#include <crow.h>
#include <crow/json.h>
#include <database/Database.hpp>
#include "core/Corrector.hpp"
#include "core/Line.hpp"
#include "core/Session.hpp"
#include "core/WagnerFischer.hpp"
#include "core/jsonify.hpp"

#define CORRECTION_ROUTE_ROUTE_1 "/books/<int>/pages/<int>/lines/<int>/correct"
#define CORRECTION_ROUTE_ROUTE_2 \
	"/books/<int>/pages/<int>/lines/<int>/words/<int>/correct"

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
const char* CorrectionRoute::route_ =
    CORRECTION_ROUTE_ROUTE_1 "," CORRECTION_ROUTE_ROUTE_2;
const char* CorrectionRoute::name_ = "CorrectionRoute";

////////////////////////////////////////////////////////////////////////////////
void CorrectionRoute::Register(App& app) {
	CROW_ROUTE(app, CORRECTION_ROUTE_ROUTE_1).methods("POST"_method)(*this);
	CROW_ROUTE(app, CORRECTION_ROUTE_ROUTE_2).methods("POST"_method)(*this);
}

////////////////////////////////////////////////////////////////////////////////
Route::Response CorrectionRoute::impl(HttpPost, const Request& req, int pid,
				      int p, int lid) const {
	const auto json = crow::json::load(req.body);
	const auto c = get<std::string>(json, "correction");
	if (not c)
		THROW(BadRequest, "(CorrectionRoute) missing correction data");
	LockedSession session(must_find_session(req));
	auto conn = must_get_connection();
	session->assert_permission(conn, pid, Permissions::Write);
	auto line = session->must_find(conn, pid, p, lid);
	return correct(conn, *line, *c);
}

////////////////////////////////////////////////////////////////////////////////
Route::Response CorrectionRoute::impl(HttpPost, const Request& req, int pid,
				      int p, int lid, int tid) const {
	const auto json = crow::json::load(req.body);
	const auto c = get<std::string>(json, "correction");
	if (not c)
		THROW(BadRequest, "(CorrectionRoute) missing correction data");
	LockedSession session(must_find_session(req));
	auto conn = must_get_connection();
	session->assert_permission(conn, pid, Permissions::Write);
	auto line = session->must_find(conn, pid, p, lid);
	return correct(conn, *line, tid, *c);
}

////////////////////////////////////////////////////////////////////////////////
Route::Response CorrectionRoute::correct(MysqlConnection& conn, Line& line,
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
Route::Response CorrectionRoute::correct(MysqlConnection& conn, Line& line,
					 int tid, const std::string& c) const {
	auto token = find_token(line, tid);
	if (not token) {
		THROW(NotFound, "(CorrectionRoute) cannot find ",
		      line.page().book().id(), "-", line.page().id(), "-",
		      line.id(), "-", tid);
	}
	WagnerFischer wf;
	wf.set_ocr(line);
	wf.set_gt(c);
	const auto b = token->offset();
	const auto n = token->size();
	const auto lev = wf(b, n);
	CROW_LOG_DEBUG << "(CorrectionRoute) correction: " << c;
	CROW_LOG_DEBUG << "(CorrectionRoute) line.cor(): " << line.cor();
	CROW_LOG_DEBUG << "(CorrectionRoute) line.ocr(): " << line.ocr();
	CROW_LOG_DEBUG << "(CorrectionRoute)        lev: " << lev;
	wf.correct(line, b, n);
	token = find_token(line, tid);
	if (not token) {
		THROW(Error, "(CorrectionRoute) cannot find token ",
		      line.page().book().id(), "-", line.page().id(), "-",
		      line.id(), "-", tid, " after correction");
	}
	update_line(conn, line);
	Json j;
	return j << *token;
}

////////////////////////////////////////////////////////////////////////////////
boost::optional<Token> CorrectionRoute::find_token(const Line& line, int tid) {
	boost::optional<Token> token;
	line.each_token([&](const auto& t) {
		if (t.id == tid) {
			token = t;
		}
	});
	return token;
}

////////////////////////////////////////////////////////////////////////////////
void CorrectionRoute::update_line(MysqlConnection& conn, const Line& line) {
	MysqlCommiter commiter(conn);
	pcw::update_line(conn.db(), line);
	commiter.commit();
}
