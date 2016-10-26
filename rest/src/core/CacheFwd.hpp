#ifndef pcw_CacheFwd_hpp__
#define pcw_CacheFwd_hpp__

#include <memory>

namespace pcw {
	template<class T> class Cache;

	class User;
	using UserPtr = std::shared_ptr<User>;
	using UserCache = Cache<User>;
	using UserCachePtr = std::shared_ptr<UserCache>;

	class Book;
	using BookPtr = std::shared_ptr<Book>;
	using BookCache = Cache<Book>;
	using BookCachePtr = std::shared_ptr<BookCache>;
}

#endif // pcw_CacheFwd_hpp__
