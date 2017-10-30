#include "SessionStore.hpp"
#include "Session.hpp"
#include "SessionDirectory.hpp"

using namespace pcw;
////////////////////////////////////////////////////////////////////////////////
SessionStore::SessionStore() : sessions_(), id_register_(), mutex_() {}

////////////////////////////////////////////////////////////////////////////////
SessionSptr SessionStore::new_session(const User& user, AppCacheSptr cache,
				      const Config& config) {
	using std::end;
	using std::begin;
	SessionSptr session = nullptr;
	Lock lock(mutex_);
	// TODO: Do something more sensitive here
	if (sessions_.size() >= THRESHOLD) {
		sessions_.clear();
		id_register_.clear();
	}
	while (not session) {
		session = std::make_shared<Session>(user, config, cache);
		// Session id and directory id must be unique.
		// You cannot use the same id for sessions and their
		// directories, since SessionDirectories leak their id to the
		// public api.
		if (not id_register_.count(session->id()) and
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
SessionSptr SessionStore::find_session(const std::string& sid) const {
	using std::end;
	using std::begin;
	Lock lock(mutex_);
	auto i = sessions_.find(sid);
	return i == end(sessions_) ? nullptr : i->second;
}

////////////////////////////////////////////////////////////////////////////////
void SessionStore::delete_session(const std::string& sid) {
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
