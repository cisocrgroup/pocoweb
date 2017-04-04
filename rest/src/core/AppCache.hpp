#ifndef pcw_AppCache_hpp__
#define pcw_AppCache_hpp__

#include <memory>
#include "Cache.hpp"
#include "User.hpp"
#include "Book.hpp"
#include "Project.hpp"

namespace pcw {
	using UserCache = Cache<User>;
	using ProjectCache = Cache<Project>;
	using BookCache = Cache<Book>;
	struct AppCache;
	using CachePtr = std::shared_ptr<AppCache>;
	using CacheSptr = std::shared_ptr<AppCache>;
	struct AppCache {
		AppCache(int us, int ps, int bs)
			: users(us)
		  	, projects(ps)
			, books(bs) {}
		UserCache users;
		ProjectCache projects;
		BookCache books;
	};
}

#endif // pcw_AppCache_hpp__
