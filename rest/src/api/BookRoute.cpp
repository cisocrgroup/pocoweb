#include "core/Book.hpp"
#include <crow.h>
#include <boost/filesystem.hpp>
#include <regex>
#include "BookRoute.hpp"
#include "core/BookDirectoryBuilder.hpp"
#include "core/Package.hpp"
#include "core/Page.hpp"
#include "core/Session.hpp"
#include "core/User.hpp"
#include "core/WagnerFischer.hpp"
#include "core/jsonify.hpp"
#include "core/util.hpp"
#include "utils/Error.hpp"
#include "utils/ScopeGuard.hpp"

#define BOOK_ROUTE_ROUTE_1 "/books"
#define BOOK_ROUTE_ROUTE_2 "/books/<int>"
#define BOOK_ROUTE_ROUTE_3 "/books/<int>/<string>"

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
const char* BookRoute::route_ =
    BOOK_ROUTE_ROUTE_1 "," BOOK_ROUTE_ROUTE_2 "," BOOK_ROUTE_ROUTE_3;
const char* BookRoute::name_ = "BookRoute";

////////////////////////////////////////////////////////////////////////////////
void BookRoute::Register(App& app) {
	CROW_ROUTE(app, BOOK_ROUTE_ROUTE_1)
	    .methods("GET"_method, "POST"_method)(*this);
	CROW_ROUTE(app, BOOK_ROUTE_ROUTE_2)
	    .methods("GET"_method, "POST"_method, "DELETE"_method)(*this);
	CROW_ROUTE(app, BOOK_ROUTE_ROUTE_3)
	    .methods("GET"_method, "POST"_method)(*this);
}

////////////////////////////////////////////////////////////////////////////////
Route::Response BookRoute::impl(HttpGet, const Request& req) const {
	auto conn = connection();
	auto session = this->session(req);
	assert(conn);
	assert(session);
	SessionLock lock(*session);
	// no permissions check since all loaded projectes are owned by the user
	auto projects = select_all_projects(conn.db(), session->user());
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
	auto conn = connection();
	auto session = this->session(req);
	assert(conn);
	assert(session);
	SessionLock lock(*session);
	session->has_permission_or_throw(conn, 0, Permissions::Create);
	// create new bookdir
	const std::string file = req.url_params.get("file");
	BookDirectoryBuilder dir(config());
	ScopeGuard sg([&dir]() { dir.remove(); });
	CROW_LOG_INFO << "(BookRoute) BookDirectoryBuilder: " << dir.dir();
	if (file.empty())  // raw upload of zip file in body
		dir.add_zip_file_content(extract_content(req));
	else  // insert uploaded file
		dir.add_zip_file_path(file);
	auto book = dir.build();
	if (not book) THROW(Error, "Could not build book");
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
void BookRoute::update_book_data(Book& book, const User& user,
				 const crow::json::rvalue& data) const {
	book.set_owner(user);
	if (data["author"].s().size()) book.data.author = data["author"].s();
	if (data["title"].s().size()) book.data.title = data["title"].s();
	if (data["language"].s().size()) book.data.lang = data["language"].s();
	if (data["year"].i()) book.data.year = data["year"].i();
	if (data["uri"].s().size()) book.data.uri = data["uri"].s();
	if (data["description"].s().size())
		book.data.description = data["description"].s();
}

////////////////////////////////////////////////////////////////////////////////
Route::Response BookRoute::impl(HttpGet, const Request& req, int bid) const {
	auto conn = connection();
	auto session = this->session(req);
	assert(conn);
	assert(session);
	SessionLock lock(*session);
	session->has_permission_or_throw(conn, bid, Permissions::Read);
	auto book = session->find(conn, bid);
	if (not book) THROW(NotFound, "Could not find project id: ", bid);
	assert(book);
	Json j;
	return j << *book;
}

////////////////////////////////////////////////////////////////////////////////
Route::Response BookRoute::impl(HttpPost, const Request& req, int bid) const {
	CROW_LOG_DEBUG << "(BookRoute) body: " << req.body;
	auto data = crow::json::load(req.body);
	auto conn = connection();
	auto session = this->session(req);
	assert(conn);
	assert(session);
	SessionLock lock(*session);
	session->has_permission_or_throw(conn, bid, Permissions::Write);
	const auto view = session->find(conn, bid);
	if (not view) THROW(NotFound, "cannot project id: ", bid);
	if (not view->is_book())
		THROW(BadRequest, "cannot set parameters of project id: ", bid);
	const auto book = std::dynamic_pointer_cast<Book>(view);
	update_book_data(*book, session->user(), data);
	MysqlCommiter commiter(conn);
	update_book(conn.db(), *book);
	commiter.commit();
	Json json;
	return json << *book;
}

////////////////////////////////////////////////////////////////////////////////
Route::Response BookRoute::impl(HttpGet, const Request& req, int bid,
				const std::string& c) const {
	if (strcasestr(c.data(), "download") == 0) {
		return download(req, bid);
	} else {
		return not_found();
	}
}

////////////////////////////////////////////////////////////////////////////////
Route::Response BookRoute::impl(HttpPost, const Request& req, int bid,
				const std::string& c) const {
	CROW_LOG_DEBUG << "(BookRoute) project id: " << bid
		       << ", command: " << c;
	if (strcmp(c.data(), "split") == 0) {
		return split(req, bid);
	} else if (strcmp(c.data(), "assign") == 0) {
		return assign(req, bid);
	} else if (strcmp(c.data(), "finish") == 0) {
		return finish(req, bid);
	} else {
		return not_found();
	}
}

////////////////////////////////////////////////////////////////////////////////
Route::Response BookRoute::impl(HttpDelete, const Request& req, int bid) const {
	auto conn = connection();
	const auto session = this->session(req);
	assert(conn);
	assert(session);
	SessionLock lock(*session);
	session->has_permission_or_throw(conn, bid, Permissions::Remove);
	const auto project = session->find(conn, bid);
	if (not project)
		THROW(NotFound, "cannot remove project: no such project id: ",
		      bid);
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
	conn.db()(remove_from(pp).where(pp.projectid == pid));
	conn.db()(remove_from(p).where(p.projectid == pid));
}

////////////////////////////////////////////////////////////////////////////////
void BookRoute::remove_book(MysqlConnection& conn, const Session& session,
			    const Book& book) const {
	assert(book.is_book());
	using namespace sqlpp;
	tables::Projects p;
	tables::Textlines l;
	tables::Contents c;
	tables::Pages pgs;
	MysqlCommiter commiter(conn);
	auto pids = conn.db()(
	    select(p.projectid, p.owner).from(p).where(p.origin == book.id()));
	for (const auto& pid : pids) {
		if (static_cast<int>(pid.owner) != session.user().id()) {
			THROW(Forbidden, "cannot delete book: project id: ",
			      pid.projectid, " is not finished");
		}
		remove_project_impl(conn, pid.projectid);
		session.uncache_project(pid.projectid);
	}
	conn.db()(remove_from(c).where(c.bookid == book.id()));
	conn.db()(remove_from(l).where(l.bookid == book.id()));
	conn.db()(remove_from(pgs).where(pgs.bookid == book.id()));
	const auto dir = book.data.dir;
	boost::system::error_code ec;
	boost::filesystem::remove_all(dir, ec);
	if (ec)
		CROW_LOG_WARNING
		    << "(BookRoute) cannot remove directory: " << dir << ": "
		    << ec.message();
	session.uncache_project(book.id());
	commiter.commit();
}

////////////////////////////////////////////////////////////////////////////////
[[noreturn]] Route::Response BookRoute::download(const Request& req,
						 int bid) const {
	CROW_LOG_DEBUG << "(BookRoute::download) body: " << req.body;
	THROW(Error, "BookRoute::download: not implemented");
}

////////////////////////////////////////////////////////////////////////////////
Route::Response BookRoute::finish(const Request& req, int bid) const {
	CROW_LOG_DEBUG << "(BookRoute::finish) body: " << req.body;
	const auto data = crow::json::load(req.body);
	auto conn = connection();
	const auto session = this->session(req);
	assert(conn);
	assert(session);
	SessionLock lock(*session);
	session->has_permission_or_throw(conn, bid, Permissions::Finish);
	const auto project = session->find(conn, bid);
	if (not project)
		THROW(NotFound, "cannot finish project: no such project id: ",
		      bid);
	const auto pentry = pcw::select_project_entry(conn.db(), bid);
	if (not pentry)
		THROW(Error, "cannot finish project: no such project id: ",
		      bid);
	const auto oentry =
	    pcw::select_project_entry(conn.db(), pentry->origin);
	if (not oentry)
		THROW(Error, "cannot finish project: no such project id: ",
		      pentry->origin);
	const auto owner = session->find_user(conn, oentry->owner);
	if (not owner)
		THROW(Error, "cannot finish project: no such user id: ",
		      oentry->owner);
	using namespace sqlpp;
	tables::Projects p;
	MysqlCommiter commiter(conn);
	conn.db()(update(p)
		      .set(p.owner = owner->id())
		      .where(p.projectid == project->id()));
	project->set_owner(*owner);
	commiter.commit();
	return ok();
}

////////////////////////////////////////////////////////////////////////////////
Route::Response BookRoute::assign(const Request& req, int bid) const {
	CROW_LOG_DEBUG << "(BookRoute::assign) body: " << req.body;
	const auto data = crow::json::load(req.body);
	auto conn = connection();
	const auto session = this->session(req);
	assert(conn);
	assert(session);
	SessionLock lock(*session);
	session->has_permission_or_throw(conn, bid, Permissions::Assign);
	const auto project = session->find(conn, bid);
	if (not project)
		THROW(NotFound, "cannot not find project: no such project id: ",
		      bid);
	UserPtr user = nullptr;
	if (data.has("id"))
		user = session->find_user(conn, data["id"].i());
	else if (data.has("name"))
		user = session->find_user(conn, data["name"].s());
	else
		THROW(BadRequest, "cannot find user: missing user information");
	if (not user) THROW(BadRequest, "cannot find user: no such user");
	MysqlCommiter commiter(conn);
	using namespace sqlpp;
	tables::Projects projects;
	auto stmnt = update(projects)
			 .set(projects.owner = user->id())
			 .where(projects.projectid == project->id());
	conn.db()(stmnt);
	project->set_owner(*user);
	commiter.commit();
	return ok();
}

////////////////////////////////////////////////////////////////////////////////
Route::Response BookRoute::split(const Request& req, int bid) const {
	CROW_LOG_DEBUG << "(BookRoute::split) body: " << req.body;
	auto data = crow::json::load(req.body);
	auto conn = connection();
	auto session = this->session(req);
	assert(conn);
	assert(session);
	SessionLock lock(*session);
	session->has_permission_or_throw(conn, bid, Permissions::Split);
	auto proj = session->find(conn, bid);
	assert(proj);
	if (not proj->is_book())
		THROW(BadRequest,
		      "cannot split project: only books can be split");
	if (proj->owner().id() != session->user().id())
		THROW(Forbidden, "cannot split book: permission denied");
	const auto n = data["n"].i();
	if (n <= 0 or static_cast<size_t>(n) > proj->size())
		THROW(BadRequest, "cannot split book: invalid split number: ",
		      n);
	std::vector<ProjectPtr> projs(n);
	std::generate(begin(projs), end(projs), [&]() {
		return std::make_shared<Package>(0, session->user(),
						 proj->origin());
	});
	// project is a book so it's origin() points to itself
	if (data.has("random") and data["random"].b())
		split_random(proj->origin(), projs);
	else
		split_sequencial(proj->origin(), projs);
	MysqlCommiter commiter(conn);
	for (const auto& p : projs) {
		assert(p);
		insert_project(conn.db(), *p);
	}
	commiter.commit();
	Json j;
	return j << projs;
}

////////////////////////////////////////////////////////////////////////////////
void BookRoute::split_random(const Book& book,
			     std::vector<ProjectPtr>& projs) const {
	assert(projs.size() <= book.size());
	std::vector<PagePtr> tmp_pages(book.size());
	std::copy(begin(book), end(book), begin(tmp_pages));
	// std::random_device rd;
	std::mt19937 g(genseed());
	std::shuffle(begin(tmp_pages), end(tmp_pages), g);
	const auto n = projs.size();
	for (auto i = 0U; i < tmp_pages.size(); ++i) {
		assert(projs[i % n]);
		projs[i % n]->push_back(*tmp_pages[i]);
	}
	for (auto& p : projs) {
		assert(p);
		std::sort(begin(*p), end(*p), [](const auto& a, const auto& b) {
			assert(a);
			assert(b);
			return a->id() < b->id();
		});
	}
}

////////////////////////////////////////////////////////////////////////////////
void BookRoute::split_sequencial(const Book& book,
				 std::vector<ProjectPtr>& projs) const {
	assert(projs.size() <= book.size());
	const auto n = projs.size();
	const auto d = book.size() / n;
	auto r = book.size() % n;
	std::vector<PagePtr> tmp_pages(book.size());
	std::copy(begin(book), end(book), begin(tmp_pages));
	size_t o = 0;
	for (auto i = 0U; i < n; ++i) {
		assert(projs[i]);
		for (auto j = 0U; j < (d + r); ++j) {
			projs[i]->push_back(*tmp_pages[j + o]);
		}
		o += (d + r);
		if (r > 0) --r;
	}
}
