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

////////////////////////////////////////////////////////////////////////////////
template <class R> size_t append_page_ids(R &rows, Json &json);

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
  // no permissions check since all loaded projectes are owned by the user
  const auto projects = select_all_projects(conn.db(), session->user());
  CROW_LOG_DEBUG << "(BookRoute) Loaded " << projects.size() << " projects";
  Json j;
  j["books"] = crow::json::rvalue(crow::json::type::List);
  size_t i = 0;
  for (const auto &p : projects) {
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
    // book->data.profilerUrl = "local";
    // update book data
    update_book_data(*book, json);
  } else {
    dir.add_zip_file_content(extract_content(req));
    book = dir.build();
    if (not book)
      THROW(BadRequest, "(BookRoute) could not build book");
    // book->data.profilerUrl = "local";
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
Route::Response BookRoute::impl(HttpGet, const Request &req, int bid) const {
  LockedSession session(get_session(req));
  auto conn = must_get_connection();
  using namespace sqlpp;
  tables::Projects projects;
  tables::Books books;
  auto rows1 = conn.db()(
      select(all_of(books), projects.id)
          .from(books.join(projects).on(projects.origin == books.bookid))
          .where(projects.id == bid));
  if (rows1.empty()) {
    THROW(NotFound, "cannot find project or package id: ", bid);
  }
  Json j;
  const auto isBook = rows1.front().bookid == rows1.front().id;
  j["bookId"] = rows1.front().bookid;
  j["projectId"] = rows1.front().id;
  j["isBook"] = isBook;
  j["year"] = rows1.front().year;
  j["author"] = rows1.front().author;
  j["title"] = rows1.front().title;
  j["language"] = rows1.front().lang;
  j["description"] = rows1.front().description;
  j["profilerUrl"] = rows1.front().profilerurl;
  j["status"]["profiled"] = rows1.front().profiled;
  j["status"]["extended-lexicon"] = rows1.front().extendedlexicon;
  j["status"]["post-corrected"] = rows1.front().postcorrected;

  tables::ProjectPages ppages;
  auto rows2 = conn.db()(select(ppages.pageid)
                             .from(ppages)
                             .where(ppages.projectid == bid)
                             .order_by(ppages.pageid.asc()));
  const auto n = append_page_ids(rows2, j);
  j["pages"] = n;
  if (n == 0 and isBook) { // no pages, but is a book - maybe pages where (not
                           // yet) inserted into project pages
    tables::Pages pages;
    auto rows3 = conn.db()(select(pages.pageid)
                               .from(pages)
                               .where(pages.bookid == bid)
                               .order_by(pages.pageid.asc()));
    j["pages"] = append_page_ids(rows3, j);
  }
  return j;
}

////////////////////////////////////////////////////////////////////////////////
template <class R> size_t append_page_ids(R &rows, Json &json) {
  json["pageIds"] = std::vector<int>();
  size_t i = 0;
  for (const auto &row : rows) {
    json["pageIds"][i] = row.pageid;
    i++;
  }
  return i;
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
  MysqlCommiter commiter(conn);
  update_book(conn.db(), *book);
  commiter.commit();
  Json json;
  return json << *book;
}

////////////////////////////////////////////////////////////////////////////////
Route::Response BookRoute::impl(HttpDelete, const Request &req, int bid) const {
  const LockedSession session(get_session(req));
  auto conn = must_get_connection();
  const auto project = session->must_find(conn, bid);
  MysqlCommiter commiter(conn);
  delete_project(conn.db(), project->id());
  if (project->is_book()) {
    const auto dir = project->origin().data.dir;
    CROW_LOG_INFO << "(BookRoute) removing directory: " << dir;
    boost::system::error_code ec;
    boost::filesystem::remove_all(dir, ec);
    if (ec)
      CROW_LOG_WARNING << "(BookRoute) cannot remove directory: " << dir << ": "
                       << ec.message();
  }
  session->uncache_project(project->id());
  commiter.commit();
  return ok();
}

#ifdef foo
////////////////////////////////////////////////////////////////////////////////
void BookRoute::remove_project(MysqlConnection &conn, const Session &session,
                               const Project &project) const {
  assert(not project.is_book());
  CROW_LOG_DEBUG << "(BookRoute) removing project id: " << project.id();
  MysqlCommiter commiter(conn);
  remove_project_impl(conn, project.id());
  session.uncache_project(project.id());
  commiter.commit();
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
  MysqlCommiter commiter(conn);
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
  commiter.commit();
}
#endif
