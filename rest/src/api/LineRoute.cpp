#include <regex>
#include <crow.h>
#include <unicode/uchar.h>
#include <utf8.h>
#include "LineRoute.hpp"
#include "core/Book.hpp"
#include "core/BookDirectoryBuilder.hpp"
#include "core/Package.hpp"
#include "core/Page.hpp"
#include "core/Session.hpp"
#include "core/User.hpp"
#include "core/WagnerFischer.hpp"
#include "core/jsonify.hpp"
#include "utils/Error.hpp"
#include "utils/ScopeGuard.hpp"

#define LINE_ROUTE_ROUTE "/books/<int>/pages/<int>/lines/<int>"

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
const char* LineRoute::route_ = LINE_ROUTE_ROUTE;
const char* LineRoute::name_ = "LineRoute";

////////////////////////////////////////////////////////////////////////////////
void
LineRoute::Register(App& app)
{
	CROW_ROUTE(app, LINE_ROUTE_ROUTE).methods("POST"_method, "GET"_method)(*this);
}

////////////////////////////////////////////////////////////////////////////////
Route::Response
LineRoute::impl(HttpGet, const Request& req, int bid, int pid, int lid) const
{
	auto conn = connection();
	auto session = this->session(req);
	assert(conn);
	assert(session);
	SessionLock lock(*session);

	auto line = session->find(conn, bid, pid, lid);
	if (not line)
		return not_found();
	assert(line);
	Json j;
	return j << *line;
}

////////////////////////////////////////////////////////////////////////////////
Route::Response
LineRoute::impl(HttpPost, const Request& req, int bid, int pid, int lid) const
{
	CROW_LOG_DEBUG << "(LineRoute) body: " << req.body;
	auto json = crow::json::load(req.body);
	if (not json["d"].s().size())
		THROW(BadRequest, "missing data for book id: ",
				bid, ", page id: ", pid, ", line id: ", lid);

	auto conn = connection();
	auto session = this->session(req);
	assert(conn);
	assert(session);
	SessionLock lock(*session);

	auto line = session->find(conn, bid, pid, lid);
	if (not line)
		THROW(NotFound, "Cannot find book id: ", bid,
				" page id: ", pid, " line id: ", lid);
	return correct(conn, *line, json);
}


////////////////////////////////////////////////////////////////////////////////
Route::Response
LineRoute::correct(MysqlConnection& conn, Line& line, const crow::json::rvalue& json) const
{
	assert(json["d"].s().size());
	const std::string correction = json["d"].s();
	WagnerFischer wf;
	wf.set_gt(correction);
	wf.set_ocr(line);
	const auto lev = wf();
	CROW_LOG_DEBUG << "(LineRoute) correction: " << correction;
	CROW_LOG_DEBUG << "(LineRoute) line.cor(): " << line.cor();
	CROW_LOG_DEBUG << "(LineRoute) line.ocr(): " << line.ocr();
	CROW_LOG_DEBUG << "(LineRoute)        lev: " << lev;

	wf.correct(line);
	log(wf);

	MysqlCommiter commiter(conn);
	update_line(conn.db(), line);
	commiter.commit();

	Json j;
	return j << line;
}

////////////////////////////////////////////////////////////////////////////////
void
LineRoute::log(const WagnerFischer& wf) const
{
	std::string u8;
	print_with_dotted_circles(wf.gt(), u8);
	CROW_LOG_INFO << "(LineRoute)    gt: " << u8;
	CROW_LOG_INFO << "(LineRoute) trace: " << wf.trace();
	print_with_dotted_circles(wf.ocr(), u8);
	CROW_LOG_INFO << "(LineRoute)   ocr: " << u8;
}

////////////////////////////////////////////////////////////////////////////////
void
LineRoute::print_with_dotted_circles(const std::wstring& str, std::string& u8)
{
	static const wchar_t dotted_circle = 0x25cc;
	u8.clear();
	for (auto c: str) {
		if (u_charType(c) == U_NON_SPACING_MARK)
			utf8::utf32to8(&dotted_circle, &dotted_circle + 1, std::back_inserter(u8));
		utf8::utf32to8(&c, &c + 1, std::back_inserter(u8));
	}
}
