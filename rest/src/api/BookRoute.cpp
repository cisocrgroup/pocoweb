#include "BookRoute.hpp"
#include "core/Book.hpp"
#include "core/BookDirectoryBuilder.hpp"
#include "core/Package.hpp"
#include "core/Page.hpp"
#include "core/Session.hpp"
#include "core/WagnerFischer.hpp"
#include "core/jsonify.hpp"
#include "core/util.hpp"
#include "database/DbStructs.hpp"
#include "utils/Error.hpp"
#include "utils/ScopeGuard.hpp"
#include <boost/filesystem.hpp>
#include <crow.h>
#include <random>
#include <regex>

#define BOOK_ROUTE_ROUTE_1 "/books"
#define BOOK_ROUTE_ROUTE_2 "/books/<int>"

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
template <class D> void update_book_data(pcw::Book &book, const D &data);
template <class Row> Json &set_book(Json &json, const Row &row);

////////////////////////////////////////////////////////////////////////////////
const char *BookRoute::route_ = BOOK_ROUTE_ROUTE_1 "," BOOK_ROUTE_ROUTE_2;
const char *BookRoute::name_ = "BookRoute";

////////////////////////////////////////////////////////////////////////////////
void BookRoute::Register(App &app) {
  CROW_ROUTE(app, BOOK_ROUTE_ROUTE_1)
      .methods("GET"_method, "POST"_method)(*this);
  CROW_ROUTE(app, BOOK_ROUTE_ROUTE_2)
      .methods("GET"_method, "POST"_method, "PUT"_method,
               "DELETE"_method)(*this);
}

////////////////////////////////////////////////////////////////////////////////
// GET /books
////////////////////////////////////////////////////////////////////////////////
Route::Response BookRoute::impl(HttpGet, const Request &req) const {
  const auto uid = get<int>(req.url_params, "userid");
  if (not uid) {
    THROW(BadRequest, "(BookRoute) Missing userid");
  }
  CROW_LOG_INFO << "(BookRoute) GET packages for user: " << uid.value();
  auto conn = must_get_connection();
  using namespace sqlpp;
  tables::Projects projects;
  tables::ProjectPages pp;
  tables::Books books;
  auto rows = conn.db()(select(all_of(projects), all_of(books), pp.pageid)
                            .from(projects.join(books)
                                      .on(projects.origin == books.bookid)
                                      .join(pp)
                                      .on(pp.projectid == projects.id))
                            .where(projects.owner == uid.value())
                            .order_by(projects.id.asc(), pp.pageid.asc()));
  // build packages with their page ids
  std::vector<DbPackage> packages;
  auto pid = -1;
  for (const auto &row : rows) {
    if (pid != row.id) {
      packages.emplace_back(int(row.id));
      load_from_row(row, packages.back());
      pid = row.id;
    }
    packages.back().pageids.push_back(row.pageid);
  }
  // build json response
  Json j;
  j["books"] = crow::json::rvalue(crow::json::type::List);
  auto i = 0;
  for (const auto &package : packages) {
    j["books"][i++] << package;
  }
  return j;
}

////////////////////////////////////////////////////////////////////////////////
// POST /books
////////////////////////////////////////////////////////////////////////////////
Route::Response BookRoute::impl(HttpPost, const Request &req) const {
  CROW_LOG_INFO << "(BookRoute) POST book";
  auto uid = get<int>(req.url_params, "userid");
  if (not uid) {
    THROW(BadRequest, "(BookRoute) missing userid");
  }
  if (crow::get_header_value(req.headers, "Content-Type") !=
      "application/zip") {
    THROW(BadRequest, "(BookRoute) invalid Content-Type: ",
          crow::get_header_value(req.headers, "Content-Type"));
  }
  // LockedSession session(get_session(req));
  auto conn = must_get_connection();
  // create new bookdir
  BookDirectoryBuilder dir(get_config());
  ScopeGuard sg([&dir]() { dir.remove(); });
  dir.add_zip_file_content(extract_content(req));
  CROW_LOG_INFO << "(BookRoute) BookDirectoryBuilder: " << dir.dir();
  auto book = dir.build();
  if (not book) {
    THROW(BadRequest, "(BookRoute) could not build book");
  }
  update_book_data(*book, req.url_params);
  book->set_owner(uid.value());
  // insert book into database
  CROW_LOG_INFO << "(BookRoute) Inserting a new book into the database";
  MysqlCommitter committer(conn);
  insert_book(conn.db(), *book);
  CROW_LOG_INFO << "(BookRoute) Created a new book id: " << book->id();
  // update and clean up
  committer.commit();
  sg.dismiss();
  Json j;
  return j << *book;

  // if (crow::get_header_value(req.headers, "Content-Type") ==
  //     "application/json") {
  //   // CROW_LOG_DEBUG << "(BookRoute) body: " << req.body;
  //   const auto json = crow::json::load(req.body);
  //   const auto file = get<std::string>(json, "file");
  //   if (not file)
  //     THROW(BadRequest, "(BookRoute) missing file parameter");
  //   dir.add_zip_file_path(*file);
  //   book = dir.build();
  //   if (not book)
  //     THROW(BadRequest, "(BookRoute) could not build book");
  //   book->set_owner(session->user());
  //   // update book data
  //   update_book_data(*book, json);
  // } else {
  //   dir.add_zip_file_content(extract_content(req));
  //   book = dir.build();
  //   if (not book)
  //     THROW(BadRequest, "(BookRoute) could not build book");
  //   book->set_owner(session->user());
  // }
  // // insert book into database
  // CROW_LOG_INFO << "(BookRoute) Inserting a new book into database";
  // MysqlCommitter committer(conn);
  // insert_book(conn.db(), *book);
  // CROW_LOG_INFO << "(BookRoute) Created a new book id: " << book->id();
  // // update and clean up
  // committer.commit();
  // sg.dismiss();
  // Json j;
  // Response response(j << *book);
  // response.code = created().code;
  // return response;
}

////////////////////////////////////////////////////////////////////////////////
template <class T, class D>
void set_if_set(T &t, const D &data, const char *key) {
  t = get<T>(data, key).value_or(t);
}

////////////////////////////////////////////////////////////////////////////////
template <class D> void update_book_data(pcw::Book &book, const D &data) {
  set_if_set(book.data.author, data, "author");
  set_if_set(book.data.title, data, "title");
  set_if_set(book.data.lang, data, "language");
  set_if_set(book.data.uri, data, "uri");
  set_if_set(book.data.description, data, "description");
  set_if_set(book.data.histPatterns, data, "histPatterns");
  set_if_set(book.data.profilerUrl, data, "profilerUrl");
  set_if_set(book.data.year, data, "year");
}

////////////////////////////////////////////////////////////////////////////////
template <class T> typename T::_cpp_value_type empty_if_null(const T &t) {
  if (not t.is_null()) {
    return t;
  } else {
    return typename T::_cpp_value_type();
  }
}
////////////////////////////////////////////////////////////////////////////////
template <class Row> Json &set_book(Json &json, const Row &row) {
  json["bookId"] = empty_if_null(row.bookid);
  json["projectId"] = empty_if_null(row.id);
  json["isBook"] = static_cast<bool>(row.bookid == row.id);
  json["year"] = empty_if_null(row.year);
  json["author"] = empty_if_null(row.author);
  json["title"] = empty_if_null(row.title);
  json["language"] = empty_if_null(row.lang);
  json["uri"] = empty_if_null(row.uri);
  json["description"] = empty_if_null(row.description);
  json["histPatterns"] = empty_if_null(row.histpatterns);
  json["profilerUrl"] = empty_if_null(row.profilerurl);
  json["status"]["profiled"] = empty_if_null(row.profiled);
  json["status"]["extended-lexicon"] = empty_if_null(row.extendedlexicon);
  json["status"]["post-corrected"] = empty_if_null(row.postcorrected);
  return json;
}

////////////////////////////////////////////////////////////////////////////////
template <class Rows> size_t append_page_ids(Json &json, Rows &rows) {
  json["pageIds"] = std::vector<int>();
  size_t i = 0;
  for (const auto &row : rows) {
    json["pageIds"][i] = row.pageid;
    i++;
  }
  return i;
}

////////////////////////////////////////////////////////////////////////////////
// GET /books/<bid>
////////////////////////////////////////////////////////////////////////////////
Route::Response BookRoute::impl(HttpGet, const Request &req, int bid) const {
  CROW_LOG_DEBUG << "(BookRoute) GET package: " << bid;
  auto conn = must_get_connection();
  DbPackage package(bid);
  if (not package.load(conn)) {
    THROW(NotFound, "cannot find package id: ", bid);
  }
  Json j;
  return j << package;
}

////////////////////////////////////////////////////////////////////////////////
// PUT /books/<bid>
////////////////////////////////////////////////////////////////////////////////
Route::Response BookRoute::impl(HttpPut, const Request &req, int bid) const {
  CROW_LOG_DEBUG << "(BookRoute) PUT package: " << bid;
  return impl(HttpPost{}, req, bid);
}

////////////////////////////////////////////////////////////////////////////////
// POST /books/<bid>
////////////////////////////////////////////////////////////////////////////////
Route::Response BookRoute::impl(HttpPost, const Request &req, int bid) const {
  CROW_LOG_DEBUG << "(BookRoute) POST package: " << bid;
  auto data = crow::json::load(req.body);
  LockedSession session(get_session(req));
  auto conn = must_get_connection();
  const auto view = session->must_find(conn, bid);
  if (not view->is_book()) {
    THROW(BadRequest, "cannot set parameters of project id: ", bid);
  }
  const auto book = std::dynamic_pointer_cast<Book>(view);
  update_book_data(*book, data);
  MysqlCommitter committer(conn);
  update_book(conn.db(), *book);
  committer.commit();
  Json json;
  return json << *book;
}

////////////////////////////////////////////////////////////////////////////////
// DELETE /books/bid
////////////////////////////////////////////////////////////////////////////////
Route::Response BookRoute::impl(HttpDelete, const Request &req, int bid) const {
  CROW_LOG_DEBUG << "(BookRoute) DELETE package: " << bid;
  const LockedSession session(get_session(req));
  auto conn = must_get_connection();
  const auto project = session->must_find(conn, bid);
  MysqlCommitter committer(conn);
  delete_project(conn.db(), project->id());
  if (project->is_book()) {
    const auto dir = project->origin().data.dir;
    CROW_LOG_INFO << "(BookRoute) removing directory: " << dir;
    boost::system::error_code ec;
    boost::filesystem::remove_all(dir, ec);
    if (ec) {
      CROW_LOG_WARNING << "(BookRoute) cannot remove directory: " << dir << ": "
                       << ec.message();
    }
  }
  session->uncache_project(project->id());
  committer.commit();
  return ok();
}
