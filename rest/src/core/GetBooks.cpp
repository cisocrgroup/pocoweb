#include <cppconn/connection.h>
#include <regex>
#include <crow.h>
#include "jsonify.hpp"
#include "BadRequest.hpp"
#include "User.hpp"
#include "Page.hpp"
#include "Book.hpp"
#include "Database.hpp"
#include "GetBooks.hpp"
#include "Sessions.hpp"
#include "SubProject.hpp"
#include "ScopeGuard.hpp"
#include "BookDir.hpp"
#include "WagnerFischer.hpp"

#define GET_BOOKS_ROUTE_0 "/books"
#define GET_BOOKS_ROUTE_1 "/books/<int>"
#define GET_BOOKS_ROUTE_2 "/books/<int>/pages"
#define GET_BOOKS_ROUTE_3 "/books/<int>/pages/<int>"
#define GET_BOOKS_ROUTE_4 "/books/<int>/pages/<int>/lines"
#define GET_BOOKS_ROUTE_5 "/books/<int>/pages/<int>/lines/<int>"


using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
const char* GetBooks::route_ = 
	GET_BOOKS_ROUTE_0 ","
	GET_BOOKS_ROUTE_1 ","
	GET_BOOKS_ROUTE_2 ","
	GET_BOOKS_ROUTE_3 ","
	GET_BOOKS_ROUTE_4 ","
	GET_BOOKS_ROUTE_5;
const char* GetBooks::name_ = "GetBooks";

////////////////////////////////////////////////////////////////////////////////
void
GetBooks::Register(App& app)
{
	CROW_ROUTE(app, GET_BOOKS_ROUTE_0)
		.methods("GET"_method)
	(*this);
	CROW_ROUTE(app, GET_BOOKS_ROUTE_1)
		.methods("GET"_method)
		.methods("POST"_method)
	(*this);
	CROW_ROUTE(app, GET_BOOKS_ROUTE_2).methods("GET"_method)(*this);
	CROW_ROUTE(app, GET_BOOKS_ROUTE_3).methods("GET"_method)(*this);
	CROW_ROUTE(app, GET_BOOKS_ROUTE_4).methods("GET"_method)(*this);
	CROW_ROUTE(app, GET_BOOKS_ROUTE_5)
		.methods("GET"_method)
		.methods("PUT"_method)
	(*this);
}

////////////////////////////////////////////////////////////////////////////////
Route::Response
GetBooks::operator()(const Request& req) const
{
	auto db = this->database(req);
	if (not db)
		return forbidden();

	std::lock_guard<std::mutex> lock(db->session().mutex);
	auto projects = db->select_all_projects(*db->session().user);
	CROW_LOG_DEBUG << "Loaded " << projects.size() << " projects";

	Json j;
	size_t i = 0;
	for (const auto& p: projects) {
		assert(p);
		j["books"][i++] << *p;
	}
	return j;
}

////////////////////////////////////////////////////////////////////////////////
Route::Response
GetBooks::operator()(const Request& req, int bid) const
{
	switch (req.method) {
	case crow::HTTPMethod::Get:
		return get(req, bid);
	case crow::HTTPMethod::Post:
		return post(req, bid);
	default:
		return not_found();
	}
}

////////////////////////////////////////////////////////////////////////////////
Route::Response
GetBooks::operator()(const Request& req, int bid, int pid) const
{
	auto db = this->database(req);
	if (not db)
		return forbidden();
	std::lock_guard<std::mutex> lock(db->session().mutex);
	auto page = find(*db, bid, pid);
	// TODO missing authentication
	Json j;
	return j << *page;
}

////////////////////////////////////////////////////////////////////////////////
Route::Response
GetBooks::operator()(const Request& req, int bid, int pid, int lid) const
{
	auto db = database(req);
	if (not db)
		return forbidden();
	std::lock_guard<std::mutex> lock(db->session().mutex);
	auto page = find(*db, bid, pid);
	if (not page)
		return not_found();
	if (not page->contains(lid))
		return not_found();

	// TODO authentication 

	switch (req.method) {
	case crow::HTTPMethod::Get:
		return get((*page)[lid]);
	case crow::HTTPMethod::Put:
		return put(req, *db, (*page)[lid]);
	default:
		return not_found();
	}
}

////////////////////////////////////////////////////////////////////////////////
Route::Response
GetBooks::get(const Request& req, int bid) const
{
	auto db = this->database(req);
	if (not db)
		return forbidden();
	std::lock_guard<std::mutex> lock(db->session().mutex);
	auto project = db->select_project(bid);
	if (not project)
		return not_found();
	// TODO missing authentication
	Json j;
	return j << *project;
}

////////////////////////////////////////////////////////////////////////////////
Route::Response
GetBooks::post(const Request& req, int bid) const
{
	auto db = database(req);
	if (not db)
		return forbidden();
	auto proj = db->select_project(bid);
	if (not proj)
		return not_found();
	auto user = db->session().user;

	if (not user)
		return internal_server_error();	

	const size_t n = 5;
	std::vector<ProjectPtr> projs(n);
	std::generate(begin(projs), end(projs), [&proj,&user]() {
		return std::make_shared<SubProject>(0, *user, proj->origin());
	});
	size_t i = 0;
	for (const auto& p: *proj) {
		projs[i++ % n]->push_back(p);
	}
	db->set_autocommit(false);
	for (const auto& p: projs) {
		assert(p);
		db->insert_project(*p);
	}
	db->commit();
	return created();
}

////////////////////////////////////////////////////////////////////////////////
Route::Response 
GetBooks::get(const Line& line) const
{
	Json j;
	return j << line;
}

////////////////////////////////////////////////////////////////////////////////
Route::Response 
GetBooks::put(const Request& req, Database& db, Line& line) const
{
	auto correction = req.url_params.get("correction");
	if (not correction)
		return bad_request();
	CROW_LOG_DEBUG << "(GetBooks) correction: " << req.url_params.get("correction");
	WagnerFischer wf;
	auto lev = wf(correction, line);
	CROW_LOG_DEBUG << "(GetBooks) lev: " << lev << "\n" << wf;
	CROW_LOG_DEBUG << "(GetBooks) line: " << line.string();
	line.correct(wf);
	CROW_LOG_DEBUG << "(GetBooks) line: " << line.string();
	db.set_autocommit(false);
	db.update_line(line);
	db.commit();
	return ok();
}

////////////////////////////////////////////////////////////////////////////////
ProjectPtr 
GetBooks::find(const Database& db, int bid) const
{
	return db.select_project(bid);
}

////////////////////////////////////////////////////////////////////////////////
PagePtr 
GetBooks::find(const Database& db, int bid, int pid) const
{
	auto book = find(db, bid);
	return book ? book->find(pid) : nullptr;
}
