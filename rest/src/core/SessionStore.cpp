#include "Session.hpp"
#include "SessionStore.hpp"

using namespace pcw;
////////////////////////////////////////////////////////////////////////////////
SessionStore::SessionStore() : sessions_(), mutex_() {}

////////////////////////////////////////////////////////////////////////////////
SessionSptr SessionStore::new_session(const User& user, AppCacheSptr cache) {
	using std::end;
	using std::begin;
	SessionSptr session = nullptr;
	Lock lock(mutex_);
	while (not session) {
		session = std::make_shared<Session>(user, cache);
		auto i = sessions_.find(session->id());
		if (i == end(sessions_)) {
			sessions_.emplace_hint(i, session->id(), session);
			return session;
		}
		session = nullptr;
	}
	assert(false);
	return nullptr;
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
	const auto b = begin(sessions_);
	const auto e = end(sessions_);
	for (auto i = b; i != e; ++i) {
		if (i->second->id() == sid) {
			sessions_.erase(i);
			break;
		}
	}
}
