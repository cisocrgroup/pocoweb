#ifndef pcw_CacheFwd_hpp__
#define pcw_CacheFwd_hpp__

#include <memory>

namespace pcw {
	template<class T> class Cache;

	class User;
	using UserPtr = std::shared_ptr<User>;
	using UserCache = Cache<User>;
	using UserCachePtr = std::shared_ptr<UserCache>;

	class Project;
	using ProjectPtr = std::shared_ptr<Project>;
	using ProjectCache = Cache<Project>;
	using ProjectCachePtr = std::shared_ptr<ProjectCache>;
}

#endif // pcw_CacheFwd_hpp__
