#include <regex>
#include <crow.h>
#include "utils/Error.hpp"
#include "utils/ScopeGuard.hpp"
#include "core/jsonify.hpp"
#include "core/User.hpp"
#include "core/Page.hpp"
#include "core/Book.hpp"
#include "core/Package.hpp"
#include "core/BookDirectoryBuilder.hpp"
#include "core/WagnerFischer.hpp"
#include "core/Session.hpp"
#include "BookRoute.hpp"

#define BOOK_ROUTE_ROUTE_1 "/books"
#define BOOK_ROUTE_ROUTE_2 "/books/<int>"
#define BOOK_ROUTE_ROUTE_3 "/books/<int>/pages"

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
const char* BookRoute::route_ =
	BOOK_ROUTE_ROUTE_1 ","
	BOOK_ROUTE_ROUTE_2 ","
	BOOK_ROUTE_ROUTE_3;
const char* BookRoute::name_ = "BookRoute";

////////////////////////////////////////////////////////////////////////////////
void
BookRoute::Register(App& app)
{
	CROW_ROUTE(app, BOOK_ROUTE_ROUTE_1).methods("GET"_method, "POST"_method)(*this);
	CROW_ROUTE(app, BOOK_ROUTE_ROUTE_2).methods("GET"_method, "POST"_method)(*this);
	CROW_ROUTE(app, BOOK_ROUTE_ROUTE_3).methods("GET"_method)(*this);
}

////////////////////////////////////////////////////////////////////////////////
Route::Response
BookRoute::impl(HttpGet, const Request& req) const
{
	auto conn = connection();
	auto session = this->session(req);
	assert(conn);
	assert(session);
	SessionLock lock(*session);
	auto projects = select_all_projects(conn.db(), session->user());
	CROW_LOG_DEBUG << "(BookRoute) Loaded " << projects.size() << " projects";
	Json j;
	size_t i = 0;
	for (const auto& p: projects) {
		j["books"][i] << p.first;
		j["books"][i]["bookId"] = p.second.origin;
		j["books"][i]["projectId"] = p.second.projectid;
		j["books"][i]["pages"] = p.second.pages;
		j["books"][i]["isBook"] = p.second.is_book();
		i++;
	}
	return j;
}

////////////////////////////////////////////////////////////////////////////////
Route::Response
BookRoute::impl(HttpPost, const Request& req) const
{
	auto conn = connection();
	assert(conn);
	auto session = this->session(req);
	if (not session)
		THROW(BadRequest, "could not find session");
	if (not session->user().admin())
		THROW(Forbidden, "must be admin to upload books");
	assert(session);
	SessionLock lock(*session);
	// create new bookdir
	const std::string file = req.url_params.get("file");
	BookDirectoryBuilder dir(config());
	ScopeGuard sg([&dir](){dir.remove();});
	CROW_LOG_INFO << "(BookRoute) BookDirectoryBuilder: " << dir.dir();
	if (file.empty()) // raw upload of zip file in body
		dir.add_zip_file_content(extract_content(req));
	else // insert uploaded file
		dir.add_zip_file_path(file);
	auto book = dir.build();
	if (not book)
		THROW(Error, "Could not build book");
	// update book data
	CROW_LOG_DEBUG << "(BookRoute) body: " << req.body;
	auto json = crow::json::load(req.body);
	update_book_data(*book, session->user(), json);
	// insert book into database
	CROW_LOG_INFO << "(BookRoute) Inserting new book into database";
	MysqlCommiter commiter(conn);
	insert_book(conn.db(), *book);
	CROW_LOG_INFO << "(BookRoute) Created new book id: " << book->id();
	// update and clean up
	commiter.commit();
	sg.dismiss();
	Json j;
	Response response(j << *book);
	response.code = created().code;
	return response;
}

////////////////////////////////////////////////////////////////////////////////
void
BookRoute::update_book_data(Book& book, const User& user, const crow::json::rvalue& data) const
{
	book.set_owner(user);
	if (data["author"].s().size())
		book.data.author = data["author"].s();
	if (data["title"].s().size())
		book.data.title = data["title"].s();
	if (data["language"].s().size())
		book.data.lang = data["language"].s();
	if (data["year"].i())
		book.data.year = data["year"].i();
	if (data["uri"].s().size())
		book.data.uri = data["uri"].s();
	if (data["description"].s().size())
		book.data.description = data["description"].s();
}

////////////////////////////////////////////////////////////////////////////////
Route::Response
BookRoute::impl(HttpGet, const Request& req, int bid) const
{
	auto conn = connection();
	auto session = this->session(req);
	assert(conn);
	assert(session);
	SessionLock lock(*session);
	auto book = session->find(conn, bid);
	assert(book);
	Json j;
	return j << *book;
}

////////////////////////////////////////////////////////////////////////////////
Route::Response
BookRoute::impl(HttpPost, const Request& req, int bid) const
{
	CROW_LOG_DEBUG << "(BookRoute) body: " << req.body;
	auto json = crow::json::load(req.body);
	if (json["package"].s().size() and json["n"].i())
		return this->package(req, bid, json);
	else if (json["author"].s().size() or json["title"].s().size() or
			json["description"].s().size() or json["year"].s().size() or
			json["uri"].s().size() or json["language"].s().size())
		return set(req, bid, json);
	else
		return bad_request();
}

////////////////////////////////////////////////////////////////////////////////
Route::Response
BookRoute::set(const Request& req, int bid, const crow::json::rvalue& data) const
{
	auto conn = connection();
	assert(conn);
	auto session = this->session(req);
	if (not session)
		THROW(BadRequest, "could not find session");
	if (not session->user().admin())
		THROW(Forbidden, "must be admin to alter book meta data");
	assert(session);
	SessionLock lock(*session);

	const auto view = session->find(conn, bid);
	if (not view)
		THROW(NotFound, "cannot load book id: ", bid);
	if (not view->is_book())
		THROW(BadRequest, "cannot set parameters of project id: ", bid);
	const auto book = std::dynamic_pointer_cast<Book>(view);
	if (data["author"].s().size())
		book->data.author = data["author"].s();
	if (data["title"].s().size())
		book->data.title = data["title"].s();
	if (data["uri"].s().size())
		book->data.uri = data["uri"].s();
	if (data["description"].s().size())
		book->data.description = data["description"].s();
	if (data["year"].i())
		book->data.year = data["year"].i();
	if (data["language"].s().size())
		book->data.lang = data["language"].s();
	MysqlCommiter commiter(conn);
	update_book(conn.db(), *book);
	commiter.commit();
	Json json;
	return json << *book;
}


////////////////////////////////////////////////////////////////////////////////
[[noreturn]] Route::Response
BookRoute::package(const Request& req, int bid, const crow::json::rvalue& data) const
{
	THROW(Error, "BookRoute::package: not implemented");
}
/*
	assert(data.n);
	assert(data.package);
	auto conn = connection();
	auto session = this->session(req);
	assert(conn);
	assert(session);
	SessionLock lock(*session);
	auto proj = session->find(conn, bid);
	assert(proj);
	int n = atoi(data.n);
	if (n <= 0)
		THROW(BadRequest, "Invalid number of packages: ", data.n);
	std::vector<ProjectPtr> projs(n);
	std::generate(begin(projs), end(projs), [&]() {
		return std::make_shared<Package>(0, session->user(), proj->origin());
	});
	size_t i = 0;
	for (const auto& p: *proj) {
		projs[i++ % n]->push_back(*p);
	}
	MysqlCommiter commiter(conn);
	for (const auto& p: projs) {
		assert(p);
		insert_project(conn.db(), *p);
	}
	commiter.commit();
	Json j;
	return j << projs;
*/
