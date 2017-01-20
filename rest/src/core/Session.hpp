#ifndef pcw_Session_hpp__
#define pcw_Session_hpp__

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
		Session(const User& user, AppCacheSptr cache = nullptr);

		const std::string& sid() const noexcept {return sid_;}
		const User& user() const noexcept {return *user_;}

		template<class Db>
		inline void insert_project(Connection<Db>& c, BookView& view) const;
		template<class Db>
		inline BookViewSptr find_project(Connection<Db>& c, int bookid) const;
		template<class Db>
		inline UserSptr find_user(Connection<Db>& c, int userid) const;

	private:
		using Mutex = std::mutex;
		using Lock = std::lock_guard<Mutex>;

		template<class Db>
		void insert_project_impl(Connection<Db>& c, BookView& view) const;
		template<class Db>
		BookViewSptr find_project_impl(Connection<Db>& c, int bookid) const;
		template<class Db>
		BookViewSptr cached_find_project(Connection<Db>& c, int bookid) const;
		template<class Db>
		UserSptr cached_find_user(Connection<Db>& c, int userid) const;
		template<class Db>
		UserSptr find_user_impl(Connection<Db>& c, int userid) const;

		void cache(BookView& view) const;
		void cache(User& user) const;

		const std::string sid_;
		const ConstUserSptr user_;
		mutable AppCacheSptr cache_;
		mutable Mutex mutex_;
	};
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
		return pcw::select_book(c.db(), *user_, projectid);
	} else if (entry) {
		auto owner = cached_find_user(c, entry->owner);
		if (owner) {
			auto book = cached_find_project(c, entry->origin);
			if (book) {
				return select_project(c.db(), book->origin(), projectid);
			}
		}
	}
	return nullptr;
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
pcw::Session::find_user_impl(Connection<Db>& c, int userid) const
{
	return select_user(c.db(), userid);
}

////////////////////////////////////////////////////////////////////////////////
template<class Db>
pcw::BookViewSptr
pcw::Session::cached_find_project(Connection<Db>& c, int projectid) const
{
	if (cache_)
		return cache_->project.get(projectid, [&](int id) {
			return find_project_impl(c, id);
		});
	else
		return find_project_impl(c, projectid);
}

////////////////////////////////////////////////////////////////////////////////
template<class Db>
pcw::UserSptr
pcw::Session::cached_find_user(Connection<Db>& c, int userid) const
{
	if (cache_)
		return cache_->user.get(userid, [&](int id) {
			return find_user_impl(c, id);
		});
	else
		return find_user_impl(c, userid);
}

#endif // pcw_Session_hpp__
