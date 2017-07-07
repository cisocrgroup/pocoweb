#ifndef pcw_SessionStore_hpp__
#define pcw_SessionStore_hpp__

#include <memory>
#include <mutex>
#include <unordered_map>

namespace pcw {
class User;
struct AppCache;
using AppCacheSptr = std::shared_ptr<AppCache>;
class Session;
using SessionSptr = std::shared_ptr<Session>;
class SessionStore;
using SessionStoreSptr = std::shared_ptr<SessionStore>;

class SessionStore {
       public:
	SessionStore();
	size_t size() const noexcept { return sessions_.size(); }
	bool empty() const noexcept { return sessions_.empty(); }
	SessionSptr new_session(const User& user, AppCacheSptr cache);
	SessionSptr find_session(const std::string& sid) const;
	void delete_session(const std::string& sid);
	auto begin() const { return sessions_.begin(); }
	auto end() const { return sessions_.end(); }
	void clear_expired_sessions();

       private:
	using Sessions = std::unordered_map<std::string, SessionSptr>;
	using Mutex = std::mutex;
	using Lock = std::lock_guard<Mutex>;

	Sessions sessions_;
	mutable Mutex mutex_;
};
}

#endif  // pcw_SessionStore_hpp__
