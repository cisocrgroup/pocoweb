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
#define GET_BOOKS_ROUTE_6 "/books/<int>/pages/<int>/lines/<int>/<string>"


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
	CROW_ROUTE(app, GET_BOOKS_ROUTE_0).methods("GET"_method)(*this);
	CROW_ROUTE(app, GET_BOOKS_ROUTE_1)
		.methods("GET"_method)
		.methods("POST"_method)
	(*this);
	CROW_ROUTE(app, GET_BOOKS_ROUTE_2).methods("GET"_method)(*this);
	CROW_ROUTE(app, GET_BOOKS_ROUTE_3).methods("GET"_method)(*this);
	CROW_ROUTE(app, GET_BOOKS_ROUTE_4).methods("GET"_method)(*this);
	CROW_ROUTE(app, GET_BOOKS_ROUTE_5).methods("GET"_method)(*this);
	// test
	CROW_ROUTE(app, GET_BOOKS_ROUTE_6).methods("GET"_method)(*this);
}

////////////////////////////////////////////////////////////////////////////////
crow::response
GetBooks::operator()(const crow::request& req) const
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
crow::response
GetBooks::operator()(const crow::request& req, int prid) const
{
	switch (req.method) {
	case crow::HTTPMethod::Get:
		return get(req, prid);
	case crow::HTTPMethod::Post:
		return post(req, prid);
	default:
		return not_found();
	}
}

////////////////////////////////////////////////////////////////////////////////
crow::response
GetBooks::operator()(const crow::request& req, int prid, int pageid) const
{
	auto db = this->database(req);
	if (not db)
		return forbidden();
	std::lock_guard<std::mutex> lock(db->session().mutex);
	auto project = db->select_project(prid);
	if (not project)
		return not_found();
	// TODO missing authentication
	auto page = project->find(pageid);
	if (not page)
		return not_found();
	Json j;
	return j << *page;
}

////////////////////////////////////////////////////////////////////////////////
crow::response
GetBooks::operator()(const crow::request& req, int prid, int pageid, int lineid) const
{
	return not_implemented();
}

////////////////////////////////////////////////////////////////////////////////
crow::response
GetBooks::get(const crow::request& req, int prid) const
{
	auto db = this->database(req);
	if (not db)
		return forbidden();
	std::lock_guard<std::mutex> lock(db->session().mutex);
	auto project = db->select_project(prid);
	if (not project)
		return not_found();
	// TODO missing authentication
	Json j;
	return j << *project;
}

////////////////////////////////////////////////////////////////////////////////
crow::response
GetBooks::post(const crow::request& req, int prid) const
{
	auto db = database(req);
	if (not db)
		return forbidden();
	auto proj = db->select_project(prid);
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
crow::response 
GetBooks::operator()(const crow::request& req, int prid, int pageid, int lineid, const std::string& foo) const
{
	auto db = database(req);
	if (not db)
		return forbidden();
	std::lock_guard<std::mutex> lock(db->session().mutex);
	auto proj = db->select_project(prid);
	if (not proj)
		return not_found();
	auto page = proj->find(pageid);
	if (not page)
		return not_found();
	if (not page->contains(lineid))
		return bad_request();
	
	WagnerFischer wf;
	auto lev = wf(foo, (*page)[lineid]);
	CROW_LOG_DEBUG << "(GetBooks) lev: " << lev << "\n" << wf;
	CROW_LOG_DEBUG << "(GetBooks) line: " << (*page)[lineid].string();
	(*page)[lineid].correct(wf);
	CROW_LOG_DEBUG << "(GetBooks) line: " << (*page)[lineid].string();
	return ok();
}
