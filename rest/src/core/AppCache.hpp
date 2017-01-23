#ifndef pcw_AppCache_hpp__
#define pcw_AppCache_hpp__

#include <memory>
#include "Cache.hpp"
#include "User.hpp"
#include "Book.hpp"
#include "BookView.hpp"

namespace pcw {
	using UserCache = Cache<User>;
	using BookViewCache = Cache<BookView>;
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
		BookViewCache projects;
		BookCache books;
	};
}

#endif // pcw_AppCache_hpp__
