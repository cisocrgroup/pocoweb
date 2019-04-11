#include "SessionStore.hpp"
#include "Session.hpp"
#include "SessionDirectory.hpp"

using namespace pcw;
////////////////////////////////////////////////////////////////////////////////
SessionStore::SessionStore()
  : sessions_()
  , id_register_()
  , mutex_()
{}

////////////////////////////////////////////////////////////////////////////////
SessionSptr
SessionStore::new_session(int user, AppCacheSptr cache, const Config& config)
{
  using std::begin;
  using std::end;
  SessionSptr session = nullptr;
  Lock lock(mutex_);
  // TODO: Do something more sensitive here
  if (sessions_.size() >= THRESHOLD) {
    sessions_.clear();
    id_register_.clear();
  }
  while (not session) {
    session = std::make_shared<Session>(user, config, cache);
	// TODO: session directory must be unique but it is only used to
	// split images.  Maybe there is a better way
    if (//not id_register_.count(session->id()) and
        not id_register_.count(session->directory().id())) {
      id_register_.insert(session->id());
      id_register_.insert(session->directory().id());
      sessions_.emplace(session->id(), session);
      return session;
    }
    session = nullptr;
  }
  throw std::logic_error("(SessionStore) cannot create new session");
}

////////////////////////////////////////////////////////////////////////////////
SessionSptr
SessionStore::find_session(const std::string& sid) const
{
  using std::begin;
  using std::end;
  Lock lock(mutex_);
  auto i = sessions_.find(sid);
  return i == end(sessions_) ? nullptr : i->second;
}

////////////////////////////////////////////////////////////////////////////////
void
SessionStore::delete_session(const std::string& sid)
{
  using std::begin;
  using std::end;
  Lock lock(mutex_);
  const auto i = sessions_.find(sid);
  if (i != end(sessions_)) {
    const auto session = i->second;
    assert(session);
    sessions_.erase(i);
    id_register_.erase(session->id());
    id_register_.erase(session->directory().id());
  }
}
