#ifndef pcw_SessionStore_hpp__
#define pcw_SessionStore_hpp__

#include <memory>
#include <mutex>
#include <unordered_map>
#include <unordered_set>

namespace pcw {
struct AppCache;
using AppCacheSptr = std::shared_ptr<AppCache>;
class Session;
using SessionSptr = std::shared_ptr<Session>;
class SessionStore;
using SessionStoreSptr = std::shared_ptr<SessionStore>;
struct Config;

class SessionStore
{
public:
  // after THRESHOLD SESSIONS are in the store,
  // *all* sessions are deleted.
  static const size_t THRESHOLD = 5000;
  SessionStore();
  size_t size() const noexcept { return sessions_.size(); }
  bool empty() const noexcept { return sessions_.empty(); }
  SessionSptr new_session(int user, AppCacheSptr cache, const Config& config);
  SessionSptr find_session(const std::string& sid) const;
  void delete_session(const std::string& sid);
  auto begin() const { return sessions_.begin(); }
  auto end() const { return sessions_.end(); }

private:
  using Sessions = std::unordered_map<std::string, SessionSptr>;
  using IdRegister = std::unordered_set<std::string>;
  using Mutex = std::mutex;
  using Lock = std::lock_guard<Mutex>;

  Sessions sessions_;
  IdRegister id_register_;
  mutable Mutex mutex_;
};
}

#endif // pcw_SessionStore_hpp__
