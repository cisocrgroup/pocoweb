#include "BookRoute.hpp"
#include "core/Book.hpp"
#include "core/BookDirectoryBuilder.hpp"
#include "core/Package.hpp"
#include "core/Page.hpp"
#include "core/WagnerFischer.hpp"
#include "core/jsonify.hpp"
#include "core/util.hpp"
#include "database/Database.hpp"
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
template <class D> void update_book_data(DbPackage &package, const D &data);
template <class D> void update_book_data(BookData &bdata, const D &data);
template <class Row> Json &set_book(Json &json, const Row &row);
static BookData as_book_data(const DbPackage &package);

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
  rapidjson::StringBuffer buf;
  rapidjson::Writer<rapidjson::StringBuffer> writer(buf);
  writer.StartObject();
  writer.String("books");
  writer.StartArray();
  for (const auto &package : packages) {
    package.serialize(writer);
  }
  writer.EndArray();
  writer.EndObject();
  Response res(200, std::string(buf.GetString(), buf.GetSize()));
  res.set_header("Content-Type", "application/json");
  return res;
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
  update_book_data(book->data, req.url_params);
  book->set_owner(uid.value());
  const auto npages = book->size();
  size_t nlines = 0;
  for (const auto &page : *book) {
    nlines += page->size();
  }
  CROW_LOG_INFO << "(BookRoute) New book: " << npages << " pages, " << nlines
                << " lines";
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
}

////////////////////////////////////////////////////////////////////////////////
template <class T, class D>
void set_if_set(T &t, const D &data, const char *key) {
  t = get<T>(data, key).value_or(t);
}

////////////////////////////////////////////////////////////////////////////////
template <class D> void update_book_data(DbPackage &package, const D &data) {
  set_if_set(package.author, data, "author");
  set_if_set(package.title, data, "title");
  set_if_set(package.language, data, "language");
  set_if_set(package.uri, data, "uri");
  set_if_set(package.description, data, "description");
  set_if_set(package.histpatterns, data, "histPatterns");
  set_if_set(package.profilerurl, data, "profilerUrl");
  set_if_set(package.year, data, "year");
  set_if_set(package.pooled, data, "pooled");
}

////////////////////////////////////////////////////////////////////////////////
template <class D> void update_book_data(BookData &bdata, const D &data) {
  set_if_set(bdata.author, data, "author");
  set_if_set(bdata.title, data, "title");
  set_if_set(bdata.lang, data, "language");
  set_if_set(bdata.uri, data, "uri");
  set_if_set(bdata.description, data, "description");
  set_if_set(bdata.histPatterns, data, "histPatterns");
  set_if_set(bdata.profilerUrl, data, "profilerUrl");
  set_if_set(bdata.year, data, "year");
  set_if_set(bdata.pooled, data, "pooled");
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
  CROW_LOG_INFO << "(BookRoute) GET package: " << bid;
  auto conn = must_get_connection();
  DbPackage pkg(bid);
  if (not pkg.load(conn)) {
    THROW(NotFound, "cannot find package id: ", bid);
  }
  rapidjson::StringBuffer buf;
  rapidjson::Writer<rapidjson::StringBuffer> writer(buf);
  pkg.serialize(writer);
  Response res(200, std::string(buf.GetString(), buf.GetSize()));
  res.set_header("Content-Type", "application/json");
  return res;
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
  auto conn = must_get_connection();
  DbPackage package(bid);
  if (!package.load(conn)) {
    THROW(Error, "cannot load package: ", bid);
  }
  // projects but not packages cannot be updated
  if (not package.isBook()) {
    THROW(BadRequest, "cannot set parameters of a package");
  }
  update_book_data(package, crow::json::load(req.body));
  MysqlCommitter committer(conn);
  update_book(conn.db(), package.bookid, as_book_data(package));
  committer.commit();
  Json json;
  return json << package;
}

////////////////////////////////////////////////////////////////////////////////
// DELETE /books/bid
////////////////////////////////////////////////////////////////////////////////
Route::Response BookRoute::impl(HttpDelete, const Request &req, int bid) const {
  CROW_LOG_DEBUG << "(BookRoute) DELETE package: " << bid;
  auto conn = must_get_connection();
  DbPackage package(bid);
  if (!package.load(conn)) {
    THROW(Error, "cannot load package: ", bid);
  }
  MysqlCommitter committer(conn);
  delete_project(conn.db(), package.projectid);
  if (package.isBook()) {
    CROW_LOG_INFO << "(BookRoute) removing directory: " << package.directory;
    boost::system::error_code ec;
    boost::filesystem::remove_all(package.directory, ec);
    if (ec) {
      CROW_LOG_WARNING << "(BookRoute) cannot remove directory: "
                       << package.directory << ": " << ec.message();
    }
  }
  committer.commit();
  return ok();
}

////////////////////////////////////////////////////////////////////////////////
BookData as_book_data(const DbPackage &package) {
  BookData ret;
  ret.author = package.author;
  ret.title = package.title;
  ret.description = package.description;
  ret.histPatterns = package.histpatterns;
  ret.uri = package.uri;
  ret.profilerUrl = package.profilerurl;
  ret.lang = package.language;
  ret.dir = package.directory;
  ret.year = package.year;
  ret.profiled = package.profiled;
  ret.extendedLexicon = package.extendedLexicon;
  ret.postCorrected = package.postCorrected;
  return ret;
}
