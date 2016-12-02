#include <cppconn/connection.h>
#include <regex>
#include <crow.h>
#include "core/jsonify.hpp"
#include "core/Error.hpp"
#include "core/User.hpp"
#include "core/Page.hpp"
#include "core/Book.hpp"
#include "core/Database.hpp"
#include "BookRoute.hpp"
#include "core/Sessions.hpp"
#include "core/Package.hpp"
#include "core/ScopeGuard.hpp"
#include "core/BookDirectoryBuilder.hpp"
#include "core/WagnerFischer.hpp"

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
BookRoute::impl(HttpPost, const Request& req) const
{
	auto db = database(req);
	std::lock_guard<std::mutex> lock(db.session().mutex);

	// create new bookdir
	BookDirectoryBuilder dir(config());
	ScopeGuard sg([&dir](){dir.remove();});
	CROW_LOG_INFO << "(BookRoute) BookDirectoryBuilder: " << dir.dir();
	dir.add_zip_file(extract_content(req));
	auto book = dir.build();
	if (not book)
		THROW(Error, "Could not build book");
	book->set_owner(*db.session().user);

	// insert book into database
	CROW_LOG_INFO << "(BookRoute) Inserting new book into database";
	db.set_autocommit(false);
	db.insert_book(*book);
	db.commit();

	// update and clean up
	CROW_LOG_INFO << "(BookRoute) Created new book id: " << book->id();
	sg.dismiss();
	Json j;
	return j << *book;
}

////////////////////////////////////////////////////////////////////////////////
Route::Response
BookRoute::impl(HttpGet, const Request& req, int bid) const
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
BookRoute::impl(HttpPost, const Request& req, int bid) const
{
	static const std::string author("author");
	static const std::string title("title");
	static const std::string uri("uri");
	static const std::string desc("description");
	static const std::string year("year");
	static const std::string lang("language");
	static const std::string package("package");
	static const std::string n("n");

	Data data;
	data.author = req.url_params.get(author);
	data.title = req.url_params.get(title);
	data.uri = req.url_params.get(uri);
	data.desc = req.url_params.get(desc);
	data.year = req.url_params.get(year);
	data.lang = req.url_params.get(lang);
	data.package = req.url_params.get(package);
	data.n = req.url_params.get(n);

	if (data.package and data.n)
		return this->package(req, bid, data);
	else if (data.author or data.title or data.uri or
			data.desc or data.year or data.lang)
		return set(req, bid, data);
	else
		return bad_request();
}

////////////////////////////////////////////////////////////////////////////////
Route::Response
BookRoute::set(const Request& req, int bid, const Data& data) const
{
	auto db = database(req);
	std::lock_guard<std::mutex> lock(db.session().mutex);
	auto view = find(db, bid);
	assert(view);
	if (not view->is_book())
		THROW(BadRequest, "Cannot set book view id: ", bid);
	auto book = std::dynamic_pointer_cast<Book>(view);

	if (data.author)
		book->author = data.author;
	if (data.title)
		book->title = data.title;
	if (data.uri)
		book->uri = data.uri;
	if (data.desc)
		book->description = data.desc;
	if (data.year)
		book->year = atoi(data.year);
	if (data.lang)
		book->lang = data.lang;

	db.update_book(*book);
	Json json;
	return json << *book;
}


////////////////////////////////////////////////////////////////////////////////
Route::Response
BookRoute::package(const Request& req, int bid, const Data& data) const
{
	assert(data.n);
	assert(data.package);

	auto db = database(req);
	std::lock_guard<std::mutex> lock(db.session().mutex);

	auto user = db.session().user;
	if (not user)
		THROW(Error, "No user");

	auto proj = find(db, bid);
	assert(proj);

	int n = atoi(data.n);
	if (n <= 0)
		THROW(BadRequest, "Invalid number of packages: ", data.n);

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
	Json j;
	return j << projs;
}
