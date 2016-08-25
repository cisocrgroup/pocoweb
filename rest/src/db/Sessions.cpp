#include <algorithm>
#include <mysql_connection.h>
#include <cassert>
#include <thread>
#include <memory>
#include <mutex>
#include <vector>
#include <json.hpp>
#include "util/hash.hpp"
#include "Config.hpp"
#include "User.hpp"
#include "Sessions.hpp"

////////////////////////////////////////////////////////////////////////////////
pcw::Sessions::Sessions(const Config& config)
	: sessions_()
	, mutex_()
	, n_(config.daemon.sessions)
{
	sessions_.reserve(n_);
}

////////////////////////////////////////////////////////////////////////////////
pcw::SessionPtr
pcw::Sessions::new_session(const User& user, ConnectionPtr connection)
{
	SessionPtr session = nullptr;
	while (not session) {
		auto sid = gensessionid(16);
		session = std::make_shared<Session>(
			user.shared_from_this(),
			std::move(connection),
			std::move(sid)
		);
		session = insert(session);
	}
	return session;
}

////////////////////////////////////////////////////////////////////////////////
pcw::SessionPtr
pcw::Sessions::insert(SessionPtr session)
{
	assert(session);
	std::lock_guard<std::mutex> lock(mutex_);
	auto i = std::find_if(begin(sessions_), end(sessions_), [&session](const auto& s) {
		return s->sid == session->sid;
	});

	// non unique session id
	if (i != end(sessions_))
		return nullptr;

	// buffer is not full yet
	if (sessions_.size() < n_)
		sessions_.emplace_back(session);

	// buffer is full -> discard last entry;
	else
		sessions_.back() = session;
	return sessions_.back();
}

////////////////////////////////////////////////////////////////////////////////
pcw::SessionPtr
pcw::Sessions::find_session(const std::string& sid) const
{
	std::lock_guard<std::mutex> lock(mutex_);
	auto i = std::find_if(begin(sessions_), end(sessions_), [&sid](const auto& s) {
		return s->sid == sid;
	});
	if (i == end(sessions_)) {
		return nullptr;
	} else if (i == begin(sessions_)) {
		assert((*i)->sid == sid);
		return *i;
	} else {
		auto j = std::prev(i);
		std::swap(*j, *i);
		assert((*j)->sid == sid);
		return *j;
	}
}
	
