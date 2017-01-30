#ifndef pcw_Session_hpp__
#define pcw_Session_hpp__

#include <chrono>
#include <memory>
#include <mutex>
#include "database/DatabaseGuard.hpp"
#include "database/NewDatabase.hpp"
#include "database/ConnectionPool.hpp"
#include "Book.hpp"
#include "BookView.hpp"
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
	class BookView;
	using BookViewSptr = std::shared_ptr<BookView>;

	class Session {
	public:
		using TimePoint = std::chrono::time_point<std::chrono::system_clock>;
		Session(const User& user, AppCacheSptr cache = nullptr);

		const std::string& id() const noexcept {return sid_;}
		const User& user() const noexcept {return *user_;}
		const TimePoint& expiration_date() const noexcept {return expiration_date_;}
		void set_cache(AppCacheSptr cache) noexcept {cache_ = std::move(cache);}
		bool has_expired() const noexcept;
		void set_expiration_date(TimePoint tp) noexcept {
			Lock lock(mutex_);
			expiration_date_ = std::move(tp);
		}
		template<class R, class P>
		void set_expiration_date_from_now(const std::chrono::duration<R, P>& d) noexcept;

		template<class Db>
		inline void insert_project(Connection<Db>& c, BookView& view) const;
		template<class Db>
		inline BookViewSptr find_project(Connection<Db>& c, int bookid) const;
		template<class Db>
		inline UserSptr find_user(Connection<Db>& c, int userid) const;
		template<class Db>
		inline UserSptr find_user(Connection<Db>& c, const std::string& name) const;

	private:
		using Mutex = std::mutex;
		using Lock = std::lock_guard<Mutex>;

		template<class Db>
		void insert_project_impl(Connection<Db>& c, BookView& view) const;
		template<class Db>
		BookViewSptr find_project_impl(Connection<Db>& c, int projectid) const;
		template<class Db>
		BookViewSptr cached_find_project(Connection<Db>& c, int projectid) const;
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

		void cache(BookView& view) const;
		void cache(User& user) const;

		const std::string sid_;
		const ConstUserSptr user_;
		AppCacheSptr cache_;
		TimePoint expiration_date_;
		mutable Mutex mutex_;
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
inline void
pcw::Session::insert_project(Connection<Db>& c, BookView& view) const
{
	Lock lock(mutex_);
	DatabaseGuard<Db> guard(c);
	insert_project_impl(c, view);
	guard.dismiss();
}

////////////////////////////////////////////////////////////////////////////////
template<class Db>
void
pcw::Session::insert_project_impl(Connection<Db>& c, BookView& view) const
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
inline pcw::BookViewSptr
pcw::Session::find_project(Connection<Db>& c, int projectid) const
{
	Lock lock(mutex_);
	return cached_find_project(c, projectid);
}

////////////////////////////////////////////////////////////////////////////////
template<class Db>
inline pcw::BookViewSptr
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
pcw::BookViewSptr
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
	Lock lock(mutex_);
	return cached_find_user(c, name);
}

////////////////////////////////////////////////////////////////////////////////
template<class Db>
inline pcw::UserSptr
pcw::Session::find_user(Connection<Db>& c, int userid) const
{
	Lock lock(mutex_);
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
