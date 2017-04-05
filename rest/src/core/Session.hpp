#ifndef pcw_Session_hpp__
#define pcw_Session_hpp__

#include <chrono>
#include <memory>
#include <mutex>
#include "database/DatabaseGuard.hpp"
#include "database/NewDatabase.hpp"
#include "database/ConnectionPool.hpp"
#include "Book.hpp"
#include "Project.hpp"
#include "AppCache.hpp"

namespace pcw {
	template<class Db> class Connection;
	class User;
	using UserSptr = std::shared_ptr<User>;
	using ConstUserSptr = std::shared_ptr<const User>;
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
	using SessionLock = std::lock_guard<Session>;

	class Session {
	public:
		using TimePoint = std::chrono::time_point<std::chrono::system_clock>;
		using Lock = std::lock_guard<Session>;

		Session(const User& user, AppCacheSptr cache = nullptr);

		const std::string& id() const noexcept {return sid_;}
		const User& user() const noexcept {return *user_;}
		const TimePoint& expiration_date() const noexcept {return expiration_date_;}
		void lock() noexcept {mutex_.lock();}
		void unlock() noexcept {mutex_.unlock();}
		void set_cache(AppCacheSptr cache) noexcept {cache_ = std::move(cache);}
		bool has_expired() const noexcept;
		void set_expiration_date(TimePoint tp) noexcept {
			expiration_date_ = std::move(tp);
		}
		template<class R, class P>
		void set_expiration_date_from_now(const std::chrono::duration<R, P>& d) noexcept;

		template<class Db>
		inline ProjectSptr find(Connection<Db>& c, int bookid) const;
		template<class Db>
		inline std::vector<ProjectSptr> select_all_projects(Connection<Db>& c) const;
		template<class Db>
		inline PageSptr find(Connection<Db>& c, int bookid, int pageid) const;
		template<class Db>
		inline LineSptr find(Connection<Db>& c, int bookid, int pageid, int lineid) const;
		template<class Db>
		inline void insert_project(Connection<Db>& c, Project& view) const;
		template<class Db>
		inline ProjectSptr find_project(Connection<Db>& c, int bookid) const;
		template<class Db>
		inline UserSptr find_user(Connection<Db>& c, int userid) const;
		template<class Db>
		inline UserSptr find_user(Connection<Db>& c, const std::string& name) const;

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
		template<class Db>
		UserSptr cached_find_user(Connection<Db>& c, int userid) const;
		template<class Db>
		UserSptr cached_find_user(Connection<Db>& c, const std::string& name) const;
		template<class Db>
		UserSptr find_user_impl(Connection<Db>& c, int userid) const;
		template<class Db>
		UserSptr find_user_impl(Connection<Db>& c, const std::string& name) const;

		void cache(Project& view) const;
		void cache(User& user) const;

		const std::string sid_;
		const ConstUserSptr user_;
		AppCacheSptr cache_;
		TimePoint expiration_date_;
		Mutex mutex_;
		mutable ProjectSptr project_;
		mutable PagePtr page_;
	};
}

////////////////////////////////////////////////////////////////////////////////
template<class R, class P>
void
pcw::Session::set_expiration_date_from_now(const std::chrono::duration<R, P>& d) noexcept
{
	set_expiration_date(std::chrono::system_clock::now() + d);
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
	auto ids = select_all_project_ids(c.db(), *user_);
	std::vector<ProjectSptr> projects(ids.size());
	std::transform(begin(ids), end(ids), begin(projects), [&](int id) {
		return this->find(c, id);
	});
	return projects;
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
		auto page = project->find(bookid);
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
	} else if (entry) {
		auto owner = cached_find_user(c, entry->owner);
		if (owner) {
			auto book = cached_find_book(c, entry->origin);
			if (book) {
				return pcw::select_project(c.db(), book->origin(), projectid);
			}
		}
	}
	return nullptr;
}

////////////////////////////////////////////////////////////////////////////////
template<class Db>
pcw::BookSptr
pcw::Session::cached_find_book(Connection<Db>& c, int bookid) const
{
	if (cache_)
		return cache_->books.get(bookid, [&](int id) {
			return pcw::select_book(c.db(), *user_, bookid);
		});
	else
		return pcw::select_book(c.db(), *user_, bookid);
}

////////////////////////////////////////////////////////////////////////////////
template<class Db>
pcw::ProjectSptr
pcw::Session::cached_find_project(Connection<Db>& c, int projectid) const
{
	if (cache_)
		return cache_->projects.get(projectid, [&](int id) {
			return find_project_impl(c, projectid);
		});
	else
		return find_project_impl(c, projectid);
}

////////////////////////////////////////////////////////////////////////////////
template<class Db>
inline pcw::UserSptr
pcw::Session::find_user(Connection<Db>& c, const std::string& name) const
{
	return cached_find_user(c, name);
}

////////////////////////////////////////////////////////////////////////////////
template<class Db>
inline pcw::UserSptr
pcw::Session::find_user(Connection<Db>& c, int userid) const
{
	return cached_find_user(c, userid);
}

////////////////////////////////////////////////////////////////////////////////
template<class Db>
inline pcw::UserSptr
pcw::Session::find_user_impl(Connection<Db>& c, const std::string& name) const
{
	return select_user(c.db(), name);
}

////////////////////////////////////////////////////////////////////////////////
template<class Db>
inline pcw::UserSptr
pcw::Session::find_user_impl(Connection<Db>& c, int userid) const
{
	return select_user(c.db(), userid);
}

////////////////////////////////////////////////////////////////////////////////
template<class Db>
pcw::UserSptr
pcw::Session::cached_find_user(Connection<Db>& c, const std::string& name) const
{
	if (cache_)
		return cache_->users.get(name, [&](const std::string& name) {
			return find_user_impl(c, name);
		});
	else
		return find_user_impl(c, name);
}

////////////////////////////////////////////////////////////////////////////////
template<class Db>
pcw::UserSptr
pcw::Session::cached_find_user(Connection<Db>& c, int userid) const
{
	if (cache_)
		return cache_->users.get(userid, [&](int id) {
			return find_user_impl(c, id);
		});
	else
		return find_user_impl(c, userid);
}

#endif // pcw_Session_hpp__
