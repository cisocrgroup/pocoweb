#include "BookRoute.hpp"
#include <crow.h>
#include <boost/filesystem.hpp>
#include <random>
#include <regex>
#include "core/Archiver.hpp"
#include "core/Book.hpp"
#include "core/BookDirectoryBuilder.hpp"
#include "core/Package.hpp"
#include "core/Page.hpp"
#include "core/Searcher.hpp"
#include "core/Session.hpp"
#include "core/User.hpp"
#include "core/WagnerFischer.hpp"
#include "core/jsonify.hpp"
#include "core/util.hpp"
#include "utils/Error.hpp"
#include "utils/ScopeGuard.hpp"

#define BOOK_ROUTE_ROUTE_1 "/books"
#define BOOK_ROUTE_ROUTE_2 "/books/<int>"

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
const char* BookRoute::route_ = BOOK_ROUTE_ROUTE_1 "," BOOK_ROUTE_ROUTE_2;
const char* BookRoute::name_ = "BookRoute";

////////////////////////////////////////////////////////////////////////////////
void BookRoute::Register(App& app) {
	CROW_ROUTE(app, BOOK_ROUTE_ROUTE_1)
	    .methods("GET"_method, "POST"_method)(*this);
	CROW_ROUTE(app, BOOK_ROUTE_ROUTE_2)
	    .methods("GET"_method, "POST"_method, "DELETE"_method)(*this);
}

////////////////////////////////////////////////////////////////////////////////
Route::Response BookRoute::impl(HttpGet, const Request& req) const {
	LockedSession session(must_find_session(req));
	auto conn = must_get_connection();
	// no permissions check since all loaded projectes are owned by the user
	const auto projects = select_all_projects(conn.db(), session->user());
	CROW_LOG_DEBUG << "(BookRoute) Loaded " << projects.size()
		       << " projects";
	Json j;
	size_t i = 0;
	for (const auto& p : projects) {
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
Route::Response BookRoute::impl(HttpPost, const Request& req) const {
	LockedSession session(must_find_session(req));
	auto conn = must_get_connection();
	session->assert_permission(conn, 0, Permissions::Create);
	// create new bookdir
	BookDirectoryBuilder dir(get_config());
	ScopeGuard sg([&dir]() { dir.remove(); });
	CROW_LOG_INFO << "(BookRoute) BookDirectoryBuilder: " << dir.dir();
	BookSptr book;
	if (crow::get_header_value(req.headers, "Content-Type") ==
	    "application/json") {
		// CROW_LOG_DEBUG << "(BookRoute) body: " << req.body;
		const auto json = crow::json::load(req.body);
		const auto file = get<std::string>(json, "file");
		if (not file)
			THROW(BadRequest, "(BookRoute) missing file parameter");
		dir.add_zip_file_path(*file);
		book = dir.build();
		if (not book) THROW(BadRequest, "Could not build book");
		book->set_owner(session->user());
		// update book data
		update_book_data(*book, json);
	} else {
		dir.add_zip_file_content(extract_content(req));
		book = dir.build();
		if (not book) THROW(BadRequest, "Could not build book");
		book->set_owner(session->user());
	}
	// insert book into database
	CROW_LOG_INFO << "(BookRoute) Inserting a new book into database";
	MysqlCommiter commiter(conn);
	insert_book(conn.db(), *book);
	CROW_LOG_INFO << "(BookRoute) Created a new book id: " << book->id();
	// update and clean up
	commiter.commit();
	sg.dismiss();
	Json j;
	Response response(j << *book);
	response.code = created().code;
	return response;
}

////////////////////////////////////////////////////////////////////////////////
void BookRoute::update_book_data(Book& book,
				 const crow::json::rvalue& data) const {
	if (data.has("author")) book.data.author = data["author"].s();
	if (data.has("title")) book.data.title = data["title"].s();
	if (data.has("language")) book.data.lang = data["language"].s();
	if (data.has("year")) book.data.year = data["year"].i();
	if (data.has("uri")) book.data.uri = data["uri"].s();
	if (data.has("description"))
		book.data.description = data["description"].s();
	if (data.has("profilerUrl"))
		book.data.profilerUrl = data["profilerUrl"].s();
	else
		book.data.profilerUrl = "local";
}

////////////////////////////////////////////////////////////////////////////////
Route::Response BookRoute::impl(HttpGet, const Request& req, int bid) const {
	LockedSession session(must_find_session(req));
	auto conn = must_get_connection();
	session->assert_permission(conn, bid, Permissions::Read);
	auto book = session->must_find(conn, bid);
	Json j;
	return j << *book;
}

////////////////////////////////////////////////////////////////////////////////
Route::Response BookRoute::impl(HttpPost, const Request& req, int bid) const {
	// CROW_LOG_DEBUG << "(BookRoute) body: " << req.body;
	auto data = crow::json::load(req.body);
	LockedSession session(must_find_session(req));
	auto conn = must_get_connection();
	session->assert_permission(conn, bid, Permissions::Write);
	const auto view = session->must_find(conn, bid);
	if (not view->is_book())
		THROW(BadRequest, "cannot set parameters of project id: ", bid);
	const auto book = std::dynamic_pointer_cast<Book>(view);
	update_book_data(*book, data);
	MysqlCommiter commiter(conn);
	update_book(conn.db(), *book);
	commiter.commit();
	Json json;
	return json << *book;
}

////////////////////////////////////////////////////////////////////////////////
Route::Response BookRoute::impl(HttpDelete, const Request& req, int bid) const {
	const LockedSession session(must_find_session(req));
	auto conn = must_get_connection();
	session->assert_permission(conn, bid, Permissions::Remove);
	const auto project = session->must_find(conn, bid);
	if (project->is_book())
		remove_book(conn, *session, project->origin());
	else
		remove_project(conn, *session, *project);
	return ok();
}

////////////////////////////////////////////////////////////////////////////////
void BookRoute::remove_project(MysqlConnection& conn, const Session& session,
			       const Project& project) const {
	assert(not project.is_book());
	CROW_LOG_DEBUG << "(BookRoute) removing project id: " << project.id();
	MysqlCommiter commiter(conn);
	remove_project_impl(conn, project.id());
	session.uncache_project(project.id());
	commiter.commit();
}

////////////////////////////////////////////////////////////////////////////////
void BookRoute::remove_project_impl(MysqlConnection& conn, int pid) const {
	using namespace sqlpp;
	tables::Projects p;
	tables::ProjectPages pp;
	tables::Profiles ppp;
	tables::Suggestions s;
	tables::Errorpatterns e;
	tables::Adaptivetokens a;
	tables::Types t;
	conn.db()(remove_from(pp).where(pp.projectid == pid));
	conn.db()(remove_from(p).where(p.projectid == pid));
	conn.db()(remove_from(ppp).where(ppp.bookid == pid));
	conn.db()(remove_from(s).where(s.bookid == pid));
	conn.db()(remove_from(e).where(e.bookid == pid));
	conn.db()(remove_from(t).where(t.bookid == pid));
	conn.db()(remove_from(a).where(a.bookid == pid));
}

////////////////////////////////////////////////////////////////////////////////
void BookRoute::remove_book(MysqlConnection& conn, const Session& session,
			    const Book& book) const {
	assert(book.is_book());
	CROW_LOG_DEBUG << "(BookRoute) removing book id: " << book.id();
	using namespace sqlpp;
	tables::Projects p;
	tables::Textlines l;
	tables::Contents c;
	tables::Pages pgs;
	tables::Profiles ppp;
	tables::Suggestions s;
	tables::Errorpatterns e;
	tables::Types t;
	tables::Books b;
	MysqlCommiter commiter(conn);
	auto pids = conn.db()(
	    select(p.projectid, p.owner).from(p).where(p.origin == book.id()));
	for (const auto& pid : pids) {
		if (static_cast<int>(pid.owner) != session.user().id()) {
			THROW(Forbidden,
			      "cannot delete book: project id: ", pid.projectid,
			      " is not finished");
		}
		remove_project_impl(conn, pid.projectid);
		session.uncache_project(pid.projectid);
	}
	conn.db()(remove_from(c).where(c.bookid == book.id()));
	conn.db()(remove_from(l).where(l.bookid == book.id()));
	conn.db()(remove_from(pgs).where(pgs.bookid == book.id()));
	conn.db()(remove_from(ppp).where(ppp.bookid == book.id()));
	conn.db()(remove_from(s).where(s.bookid == book.id()));
	conn.db()(remove_from(e).where(e.bookid == book.id()));
	conn.db()(remove_from(t).where(t.bookid == book.id()));
	conn.db()(remove_from(b).where(b.bookid == book.id()));
	const auto dir = book.data.dir;
	CROW_LOG_INFO << "(BookRoute) removing directory: " << dir;
	boost::system::error_code ec;
	boost::filesystem::remove_all(dir, ec);
	if (ec)
		CROW_LOG_WARNING
		    << "(BookRoute) cannot remove directory: " << dir << ": "
		    << ec.message();
	session.uncache_project(book.id());
	commiter.commit();
}
