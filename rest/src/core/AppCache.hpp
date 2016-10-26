#ifndef pcw_AppCache_hpp__
#define pcw_AppCache_hpp__

#include <memory>
#include "Cache.hpp"
#include "User.hpp"
#include "Project.hpp"

namespace pcw {
	using UserCache = Cache<User>;
	using ProjectCache = Cache<Project>;
	struct AppCache;
	using CachePtr = std::shared_ptr<AppCache>;
	struct AppCache {
		AppCache(int us, int ps): user_cache(us), project_cache(ps) {}
		UserCache user_cache;
		ProjectCache project_cache;
	};
}

#endif // pcw_AppCache_hpp__
