#ifndef pcw_Session_hpp__
#define pcw_Session_hpp__

#include "AppCache.hpp"
#include "Book.hpp"
#include "Permissions.hpp"
#include "Project.hpp"
#include "database/ConnectionPool.hpp"
#include "database/Database.hpp"
#include "database/DatabaseGuard.hpp"
#include "utils/Error.hpp"
#include <chrono>
#include <memory>
#include <mutex>

namespace crow {
struct response;
struct request;
}

namespace pcw {
template<class Db>
class Connection;
class Session;
using SessionSptr = std::shared_ptr<Session>;
struct AppCache;
using AppCacheSptr = std::shared_ptr<AppCache>;
class Book;
using BookSptr = std::shared_ptr<Book>;
class Project;
using ProjectSptr = std::shared_ptr<Project>;
class Page;
using PageSptr = std::shared_ptr<Page>;
class Line;
using LineSptr = std::shared_ptr<Line>;
class Session;
using SessionPtr = std::shared_ptr<Session>;
class SessionDirectory;
using SessionDirectoryPtr = std::unique_ptr<SessionDirectory>;
struct Config;

class LockedSession
{
public:
  LockedSession(SessionPtr session)
    : session_(session)
    , lock_(*session)
  {}
  Session* operator->() noexcept { return session_.get(); }
  const Session* operator->() const noexcept { return session_.get(); }
  Session& operator*() noexcept { return *session_; }
  const Session& operator*() const noexcept { return *session_; }

private:
  SessionPtr session_;
  std::lock_guard<Session> lock_;
};

class Session
{
public:
  using TimePoint = std::chrono::time_point<std::chrono::system_clock>;
  using Lock = std::lock_guard<Session>;
  static const size_t SESSION_ID_LENGTH = 16;

  Session(int user, const Config& config, AppCacheSptr cache = nullptr);

  const std::string& id() const noexcept { return sid_; }
  int user() const noexcept { return user_; }
  void set_user(int user) noexcept;
  SessionDirectory& directory() const noexcept { return *dir_; }
  void lock() noexcept { mutex_.lock(); }
  void unlock() noexcept { mutex_.unlock(); }
  void set_cache(AppCacheSptr cache) noexcept { cache_ = std::move(cache); }
  void uncache_project(int pid) const;

  template<class Db>
  inline ProjectSptr find(Connection<Db>& c, int bookid) const;
  template<class Db>
  inline std::vector<ProjectSptr> select_all_projects(Connection<Db>& c) const;
  template<class Db>
  inline PageSptr find(Connection<Db>& c, int bookid, int pageid) const;
  template<class Db>
  inline LineSptr find(Connection<Db>& c,
                       int bookid,
                       int pageid,
                       int lineid) const;
  template<class Db>
  inline ProjectSptr must_find(Connection<Db>& c, int bookid) const;
  template<class Db>
  inline PageSptr must_find(Connection<Db>& c, int bookid, int pageid) const;
  template<class Db>
  inline LineSptr must_find(Connection<Db>& c,
                            int bookid,
                            int pageid,
                            int lineid) const;
  template<class Db>
  inline void insert_project(Connection<Db>& c, Project& view) const;
  template<class Db>
  inline ProjectSptr find_project(Connection<Db>& c, int bookid) const;

private:
  using Mutex = std::mutex;

  template<class Db>
  void insert_project_impl(Connection<Db>& c, Project& view) const;
  template<class Db>
  ProjectSptr find_project_impl(Connection<Db>& c, int projectid) const;
  template<class Db>
  ProjectSptr cached_find_project(Connection<Db>& c, int projectid) const;
  template<class Db>
  BookSptr cached_find_book(Connection<Db>& c, int bookid) const;

  void cache(Project& view) const;

  const std::string sid_;
  int user_;
  const SessionDirectoryPtr dir_;

  AppCacheSptr cache_;
  Mutex mutex_;
  mutable ProjectSptr project_;
  mutable PagePtr page_;
};
}

////////////////////////////////////////////////////////////////////////////////
template<class Db>
pcw::ProjectSptr
pcw::Session::must_find(Connection<Db>& c, int bookid) const
{
  auto p = find(c, bookid);
  if (not p)
    THROW(NotFound, "(Session) cannot find project id: ", bookid);
  return p;
}

////////////////////////////////////////////////////////////////////////////////
template<class Db>
pcw::ProjectSptr
pcw::Session::find(Connection<Db>& c, int bookid) const
{
  if (project_ and project_->id() == bookid)
    return project_;

  auto project = cached_find_project(c, bookid);
  if (project) {
    project_ = project;
  }
  return project;
}

////////////////////////////////////////////////////////////////////////////////
template<class Db>
std::vector<pcw::ProjectSptr>
pcw::Session::select_all_projects(Connection<Db>& c) const
{
  auto ids = select_all_project_ids(c.db(), user_);
  std::vector<ProjectSptr> projects(ids.size());
  std::transform(begin(ids), end(ids), begin(projects), [&](int id) {
    return this->find(c, id);
  });
  return projects;
}

////////////////////////////////////////////////////////////////////////////////
template<class Db>
pcw::PageSptr
pcw::Session::must_find(Connection<Db>& c, int bookid, int pageid) const
{
  auto p = find(c, bookid, pageid);
  if (not p)
    THROW(NotFound,
          "(Session) cannot find project id: ",
          bookid,
          ", page id: ",
          pageid);
  return p;
}

////////////////////////////////////////////////////////////////////////////////
template<class Db>
pcw::PageSptr
pcw::Session::find(Connection<Db>& c, int bookid, int pageid) const
{
  if (page_ and page_->id() == pageid and project_ and project_->id() == bookid)
    return page_;

  auto project = find(c, bookid);
  if (project) {
    auto page = project->find(pageid);
    if (page) {
      page_ = page;
      return page;
    }
  }
  return nullptr;
}

////////////////////////////////////////////////////////////////////////////////
template<class Db>
pcw::LineSptr
pcw::Session::must_find(Connection<Db>& c,
                        int bookid,
                        int pageid,
                        int lineid) const
{
  auto l = find(c, bookid, pageid, lineid);
  if (not l)
    THROW(NotFound,
          "(Session) cannot find project id: ",
          bookid,
          ", page id: ",
          pageid,
          ", line id: ",
          lineid);
  return l;
}

////////////////////////////////////////////////////////////////////////////////
template<class Db>
pcw::LineSptr
pcw::Session::find(Connection<Db>& c, int bookid, int pageid, int lineid) const
{
  if (page_ and page_->id() == pageid and project_ and project_->id() == bookid)
    return page_->find(lineid);
  auto page = find(c, bookid, pageid);
  if (page)
    return page->find(lineid);
  return nullptr;
}

////////////////////////////////////////////////////////////////////////////////
template<class Db>
inline void
pcw::Session::insert_project(Connection<Db>& c, Project& view) const
{
  DatabaseGuard<Db> guard(c);
  insert_project_impl(c, view);
  guard.dismiss();
}

////////////////////////////////////////////////////////////////////////////////
template<class Db>
void
pcw::Session::insert_project_impl(Connection<Db>& c, Project& view) const
{
  if (view.is_book()) {
    auto book = std::dynamic_pointer_cast<Book>(view.shared_from_this());
    cache(*pcw::insert_book(c.db(), *book));
  } else {
    cache(*pcw::insert_project(c.db(), view));
  }
}

////////////////////////////////////////////////////////////////////////////////
template<class Db>
inline pcw::ProjectSptr
pcw::Session::find_project(Connection<Db>& c, int projectid) const
{
  return cached_find_project(c, projectid);
}

////////////////////////////////////////////////////////////////////////////////
template<class Db>
inline pcw::ProjectSptr
pcw::Session::find_project_impl(Connection<Db>& c, int projectid) const
{
  auto entry = select_project_entry(c.db(), projectid);
  if (entry and entry->is_book()) {
    return cached_find_book(c, projectid);
  } else if (entry) { // HERE LIES THY DOOM
    auto owner = entry->owner;
    if (not owner) {
      return nullptr;
    }
    auto book = cached_find_book(c, entry->origin);
    if (not book) {
      return nullptr;
    }
    assert(book->is_book());
    return pcw::select_project(c.db(), *book, projectid);
  }
  return nullptr;
}

////////////////////////////////////////////////////////////////////////////////
template<class Db>
pcw::BookSptr
pcw::Session::cached_find_book(Connection<Db>& c, int bookid) const
{
  if (cache_) {
    return cache_->books.get(bookid, [&](int id) {
      CROW_LOG_DEBUG << "(Session) loading book id: " << bookid;
      return pcw::select_book(c.db(), user_, bookid);
    });
  } else {
    return pcw::select_book(c.db(), user_, bookid);
  }
}

////////////////////////////////////////////////////////////////////////////////
template<class Db>
pcw::ProjectSptr
pcw::Session::cached_find_project(Connection<Db>& c, int projectid) const
{
  if (cache_)
    return cache_->projects.get(
      projectid, [&](int id) { return find_project_impl(c, projectid); });
  else
    return find_project_impl(c, projectid);
}

#endif // pcw_Session_hpp__
