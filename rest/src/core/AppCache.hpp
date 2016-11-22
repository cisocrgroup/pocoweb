#ifndef pcw_AppCache_hpp__
#define pcw_AppCache_hpp__

#include <memory>
#include "Cache.hpp"
#include "User.hpp"
#include "BookView.hpp"

namespace pcw {
	using UserCache = Cache<User>;
	using BookViewCache = Cache<BookView>;
	struct AppCache;
	using CachePtr = std::shared_ptr<AppCache>;
	struct AppCache {
		AppCache(int us, int ps): user(us), project(ps) {}
		UserCache user;
		BookViewCache project;
	};
}

#endif // pcw_AppCache_hpp__
