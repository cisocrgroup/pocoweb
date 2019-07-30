#include "BookRoute.hpp"
#include "core/Archiver.hpp"
#include "core/Book.hpp"
#include "core/BookDirectoryBuilder.hpp"
#include "core/Package.hpp"
#include "core/Page.hpp"
#include "core/Session.hpp"
#include "core/WagnerFischer.hpp"
#include "core/jsonify.hpp"
#include "core/util.hpp"
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
static void update_book_data(pcw::Book &book, const crow::json::rvalue &data);
template <class Row> Json &set_book(Json &json, const Row &row);

////////////////////////////////////////////////////////////////////////////////
const char *BookRoute::route_ = BOOK_ROUTE_ROUTE_1 "," BOOK_ROUTE_ROUTE_2;
const char *BookRoute::name_ = "BookRoute";

////////////////////////////////////////////////////////////////////////////////
void BookRoute::Register(App &app) {
  CROW_ROUTE(app, BOOK_ROUTE_ROUTE_1)
      .methods("GET"_method, "POST"_method)(*this);
  CROW_ROUTE(app, BOOK_ROUTE_ROUTE_2)
      .methods("GET"_method, "POST"_method, "DELETE"_method)(*this);
}

////////////////////////////////////////////////////////////////////////////////
Route::Response BookRoute::impl(HttpGet, const Request &req) const {
  LockedSession session(get_session(req));
  auto conn = must_get_connection();
  const auto uid = session->user();
  CROW_LOG_INFO << "(BookRoute) loading projects and packages for user id: "
                << uid;
  using namespace sqlpp;
  tables::Projects projects;
  tables::Books books;
  tables::ProjectPages pages;
  auto rows = conn.db()(select(all_of(projects), all_of(books), pages.pageid)
                            .from(projects.join(books)
                                      .on(projects.origin == books.bookid)
                                      .join(pages)
                                      .on(projects.id == pages.projectid))
                            .where(projects.owner == uid)
                            .order_by(projects.id.asc()));
  Json j;
  j["books"] = crow::json::rvalue(crow::json::type::List);
  int last_id = -1;
  size_t i = -1;
  size_t p = 0;
  for (const auto &row : rows) {
    if (last_id != row.id) {
      ++i;
      p = 0;
      last_id = row.id;
      j["books"][i]["pages"] = row.pages;
      set_book(j["books"][i], row);
    }
    j["books"][i]["pageIds"][p] = row.pageid;
    ++p;
  }
  CROW_LOG_DEBUG << "(BookRoute) loaded " << i << " projects and packages";
  return j;
}

////////////////////////////////////////////////////////////////////////////////
Route::Response BookRoute::impl(HttpPost, const Request &req) const {
  LockedSession session(get_session(req));
  auto conn = must_get_connection();
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
    if (not book)
      THROW(BadRequest, "(BookRoute) could not build book");
    book->set_owner(session->user());
    // update book data
    update_book_data(*book, json);
  } else {
    dir.add_zip_file_content(extract_content(req));
    book = dir.build();
    if (not book)
      THROW(BadRequest, "(BookRoute) could not build book");
    book->set_owner(session->user());
  }
  // insert book into database
  CROW_LOG_INFO << "(BookRoute) Inserting a new book into database";
  MysqlCommitter committer(conn);
  insert_book(conn.db(), *book);
  CROW_LOG_INFO << "(BookRoute) Created a new book id: " << book->id();
  // update and clean up
  committer.commit();
  sg.dismiss();
  Json j;
  Response response(j << *book);
  response.code = created().code;
  return response;
}

////////////////////////////////////////////////////////////////////////////////
template <class T>
void set_if_set(T &t, const crow::json::rvalue &data, const char *key) {
  t = get<T>(data, key).value_or(t);
}

////////////////////////////////////////////////////////////////////////////////
void update_book_data(pcw::Book &book, const crow::json::rvalue &data) {
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
Route::Response BookRoute::impl(HttpGet, const Request &req, int bid) const {
  LockedSession session(get_session(req));
  auto conn = must_get_connection();
  using namespace sqlpp;
  tables::Projects projects;
  tables::Books books;
  tables::ProjectPages pages;
  auto rows = conn.db()(select(all_of(books), all_of(projects), pages.pageid)
                            .from(books.join(projects)
                                      .on(projects.origin == books.bookid)
                                      .join(pages)
                                      .on(pages.projectid == projects.id))
                            .where(projects.id == bid));
  if (rows.empty()) {
    THROW(NotFound, "cannot find project or package id: ", bid);
  }
  auto first = false;
  size_t p = 0;
  Json j;
  for (const auto &row : rows) {
    if (!first) {
      j["pages"] = row.pages;
      set_book(j, row);
      first = true;
    }
    j["pageIds"][p] = row.pageid;
    ++p;
  }
  return j;
}

////////////////////////////////////////////////////////////////////////////////
Route::Response BookRoute::impl(HttpPost, const Request &req, int bid) const {
  CROW_LOG_DEBUG << "(BookRoute) post book: " << bid;
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
Route::Response BookRoute::impl(HttpDelete, const Request &req, int bid) const {
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

#ifdef foo
////////////////////////////////////////////////////////////////////////////////
void BookRoute::remove_project(MysqlConnection &conn, const Session &session,
                               const Project &project) const {
  assert(not project.is_book());
  CROW_LOG_DEBUG << "(BookRoute) removing project id: " << project.id();
  MysqlCommitter committer(conn);
  remove_project_impl(conn, project.id());
  session.uncache_project(project.id());
  committer.commit();
}

////////////////////////////////////////////////////////////////////////////////
void BookRoute::remove_project_impl(MysqlConnection &conn, int pid) const {
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
void BookRoute::remove_book(MysqlConnection &conn, const Session &session,
                            const Book &book) const {
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
  MysqlCommitter committer(conn);
  auto pids = conn.db()(
      select(p.projectid, p.owner).from(p).where(p.origin == book.id()));
  for (const auto &pid : pids) {
    if (static_cast<int>(pid.owner) != session.user().id()) {
      THROW(Forbidden, "cannot delete book: project id: ", pid.projectid,
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
    CROW_LOG_WARNING << "(BookRoute) cannot remove directory: " << dir << ": "
                     << ec.message();
  session.uncache_project(book.id());
  committer.commit();
}
#endif
