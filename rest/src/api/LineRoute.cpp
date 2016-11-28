#include <cppconn/connection.h>
#include <regex>
#include <crow.h>
#include <unicode/uchar.h>
#include <utf8.h>
#include "core/jsonify.hpp"
#include "core/Error.hpp"
#include "core/User.hpp"
#include "core/Page.hpp"
#include "core/Book.hpp"
#include "core/Database.hpp"
#include "LineRoute.hpp"
#include "core/Sessions.hpp"
#include "core/Package.hpp"
#include "core/ScopeGuard.hpp"
#include "core/BookDirectoryBuilder.hpp"
#include "core/WagnerFischer.hpp"

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
	auto db = database(req);
	std::lock_guard<std::mutex> lock(db.session().mutex);
	auto line = find(db, bid, pid, lid);
	assert(line);
	Json j;
	return j << *line;
}

////////////////////////////////////////////////////////////////////////////////
Route::Response
LineRoute::impl(HttpPost, const Request& req, int bid, int pid, int lid) const
{
	Data data;
	data.correction = req.url_params.get("correction");
	data.partial = req.url_params.get("partial");
	if (not data.correction)
		return bad_request();

	auto db = database(req);
	std::lock_guard<std::mutex> lock(db.session().mutex);
	auto line = find(db, bid, pid, lid);
	if (not line)
		THROW(NotFound, "Cannot find book id: ", bid,
				" page id: ", pid, " line id: ", lid);
	return correct(db, *line, data);
}


////////////////////////////////////////////////////////////////////////////////
Route::Response
LineRoute::correct(Database& db, Line& line, const Data& data) const
{

	WagnerFischer wf;
	wf.set_gt(data.correction);
	wf.set_ocr(line);
	auto lev = wf();

	CROW_LOG_DEBUG << "(LineRoute) correction: " << data.correction;
	CROW_LOG_DEBUG << "(LineRoute) line.cor(): " << line.cor();
	CROW_LOG_DEBUG << "(LineRoute) line.ocr(): " << line.ocr();
	CROW_LOG_DEBUG << "(LineRoute)        lev: " << lev << "\n";
	wf.correct(line);
	log(wf);
	db.set_autocommit(false);
	db.update_line(line);
	db.commit();
	Json json;
	return json << line;
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
