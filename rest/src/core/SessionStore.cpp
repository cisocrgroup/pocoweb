#include "Session.hpp"
#include "SessionStore.hpp"

using namespace pcw;
////////////////////////////////////////////////////////////////////////////////
SessionStore::SessionStore()
	: sessions_()
	, mutex_()
{
}

////////////////////////////////////////////////////////////////////////////////
SessionSptr
SessionStore::new_session(const User& user, AppCacheSptr cache)
{
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
SessionSptr
SessionStore::find_session(const std::string& sid) const
{
	using std::end;
	using std::begin;
	Lock lock(mutex_);
	auto i = sessions_.find(sid);
	return i == end(sessions_) ? nullptr : i->second;
}

////////////////////////////////////////////////////////////////////////////////
void
SessionStore::clear_expired_sessions()
{
	using std::end;
	using std::begin;
	Lock lock(mutex_);
	const auto e = end(sessions_);
	for (auto i = begin(sessions_); i != e;) {
		if (i->second and i->second->has_expired())
			i = sessions_.erase(i);
		else
			++i;
	}
}
