#include <cppconn/connection.h>
#include <regex>
#include <crow.h>
#include "core/jsonify.hpp"
#include "core/Error.hpp"
#include "core/User.hpp"
#include "core/Page.hpp"
#include "core/Book.hpp"
#include "core/Database.hpp"
#include "ApiBooks.hpp"
#include "core/Sessions.hpp"
#include "core/Package.hpp"
#include "core/ScopeGuard.hpp"
#include "core/BookDirectoryBuilder.hpp"
#include "core/WagnerFischer.hpp"

#define API_BOOKS_ROUTE_0 "/books"
#define API_BOOKS_ROUTE_1 "/books/<int>"
#define API_BOOKS_ROUTE_2 "/books/<int>/pages"
#define API_BOOKS_ROUTE_3 "/books/<int>/pages/<int>"
#define API_BOOKS_ROUTE_4 "/books/<int>/pages/<int>/lines"
#define API_BOOKS_ROUTE_5 "/books/<int>/pages/<int>/lines/<int>"


using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
const char* ApiBooks::route_ =
	API_BOOKS_ROUTE_0 ","
	API_BOOKS_ROUTE_1 ","
	API_BOOKS_ROUTE_2 ","
	API_BOOKS_ROUTE_3 ","
	API_BOOKS_ROUTE_4 ","
	API_BOOKS_ROUTE_5;
const char* ApiBooks::name_ = "ApiBooks";

////////////////////////////////////////////////////////////////////////////////
void
ApiBooks::Register(App& app)
{
	CROW_ROUTE(app, API_BOOKS_ROUTE_0).methods("GET"_method, "POST"_method)(*this);
	CROW_ROUTE(app, API_BOOKS_ROUTE_1).methods("GET"_method, "POST"_method)(*this);
	CROW_ROUTE(app, API_BOOKS_ROUTE_2).methods("GET"_method)(*this);
	CROW_ROUTE(app, API_BOOKS_ROUTE_3).methods("GET"_method)(*this);
	CROW_ROUTE(app, API_BOOKS_ROUTE_4).methods("GET"_method)(*this);
	CROW_ROUTE(app, API_BOOKS_ROUTE_5).methods("GET"_method, "PUT"_method)(*this);
}

////////////////////////////////////////////////////////////////////////////////
Route::Response
ApiBooks::impl(HttpGet, const Request& req) const
{
	auto db = database(req);
	std::lock_guard<std::mutex> lock(db.session().mutex);

	auto projects = db.select_all_projects(*db.session().user);
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
ApiBooks::impl(HttpPost, const Request& req) const
{
	auto db = database(req);
	std::lock_guard<std::mutex> lock(db.session().mutex);

	// create new bookdir
	BookDirectoryBuilder dir(config());
	ScopeGuard sg([&dir](){dir.remove();});
	CROW_LOG_INFO << "(ApiBooks) BookDirectoryBuilder: " << dir.dir();
	dir.add_zip_file(extract_content(req));
	auto book = dir.build();
	if (not book)
		THROW(Error, "Could not build book");
	book->set_owner(*db.session().user);

	// insert book into database
	CROW_LOG_INFO << "(ApiBooks) Inserting new book into database";
	db.set_autocommit(false);
	db.insert_book(*book);
	db.commit();

	// update and clean up
	CROW_LOG_INFO << "(ApiBooks) Created new book id: " << book->id();
	sg.dismiss();
	return created();
}

////////////////////////////////////////////////////////////////////////////////
Route::Response
ApiBooks::impl(HttpGet, const Request& req, int bid) const
{
	auto db = database(req);
	std::lock_guard<std::mutex> lock(db.session().mutex);

	auto book = find(db, bid);
	assert(book);
	// TODO missing authentication
	Json j;
	return j << *book;
}

////////////////////////////////////////////////////////////////////////////////
Route::Response
ApiBooks::impl(HttpPost, const Request& req, int bid) const
{
	auto db = database(req);
	std::lock_guard<std::mutex> lock(db.session().mutex);

	auto user = db.session().user;
	if (not user)
		THROW(Error, "No user");

	auto proj = find(db, bid);
	const size_t n = 5;
	std::vector<BookViewPtr> projs(n);
	std::generate(begin(projs), end(projs), [&proj,&user]() {
		return std::make_shared<Package>(0, *user, proj->origin());
	});
	size_t i = 0;
	for (const auto& p: *proj) {
		projs[i++ % n]->push_back(p);
	}
	db.set_autocommit(false);
	for (const auto& p: projs) {
		assert(p);
		db.insert_project(*p);
	}
	db.commit();
	return created();
}

////////////////////////////////////////////////////////////////////////////////
Route::Response
ApiBooks::impl(HttpGet, const Request& req, int bid, int pid) const
{
	auto db = database(req);
	std::lock_guard<std::mutex> lock(db.session().mutex);
	auto page = find(db, bid, pid);
	assert(page);
	// TODO missing authentication
	Json j;
	return j << *page;
}

////////////////////////////////////////////////////////////////////////////////
Route::Response
ApiBooks::impl(HttpGet, const Request& req, int bid, int pid, int lid) const
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
ApiBooks::impl(HttpPut, const Request& req, int bid, int pid, int lid) const
{
	auto correction = req.url_params.get("correction");
	if (not correction)
		return bad_request();
	auto db = database(req);
	std::lock_guard<std::mutex> lock(db.session().mutex);
	auto line = find(db, bid, pid, lid);
	assert(line);

	CROW_LOG_DEBUG << "(ApiBooks) correction: " << req.url_params.get("correction");
	WagnerFischer wf;
	wf.set_gt(correction);
	wf.set_ocr(*line);
	auto lev = wf();
	CROW_LOG_DEBUG << "(ApiBooks) lev: " << lev << "\n" << wf;
	CROW_LOG_DEBUG << "(ApiBooks) line: " << line->cor();
	wf.correct(*line);
	CROW_LOG_DEBUG << "(ApiBooks) line: " << line->cor();
	db.set_autocommit(false);
	db.update_line(*line);
	db.commit();
	return ok();
}

////////////////////////////////////////////////////////////////////////////////
BookViewPtr
ApiBooks::find(const Database& db, int bid) const
{
	auto book = db.select_project(bid);
	if (not book)
		THROW(NotFound, "Not Found: book id ", bid);
	return book;
}

////////////////////////////////////////////////////////////////////////////////
PagePtr
ApiBooks::find(const Database& db, int bid, int pid) const
{
	auto page = find(db, bid)->find(pid);
	if (not page)
		THROW(NotFound, "Not found: book id ", bid, ", page id ", pid);
	return page;
}

////////////////////////////////////////////////////////////////////////////////
LinePtr
ApiBooks::find(const Database& db, int bid, int pid, int lid) const
{
	auto page = find(db, bid, pid);
	assert(page);
	auto line = page->find(lid);
	if (not line)
		THROW(NotFound, "Not found: book id ", bid, ", page id ", pid, ", line id ", lid);
	return line;
}
