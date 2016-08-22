#include <algorithm>
#include <cassert>
#include <thread>
#include <memory>
#include <mutex>
#include <vector>
#include <json.hpp>
#include "Config.hpp"
#include "User.hpp"
#include "Sessions.hpp"

////////////////////////////////////////////////////////////////////////////////
pcw::Sessions::Sessions(const Config& config)
	: sessions_()
	, mutex_()
	, n_(config.sessions.n)
{
	sessions_.reserve(n_);
}

////////////////////////////////////////////////////////////////////////////////
pcw::SessionPtr
pcw::Sessions::new_session(const std::string& sid)
{
	std::lock_guard<std::mutex> lock(mutex_);
	auto i = std::find_if(begin(sessions_), end(sessions_), [&sid](const auto& p) {
		return p.first == sid;
	});

	// non unique session id
	if (i != end(sessions_))
		return nullptr;

	// buffer is not full yet
	if (sessions_.size() < n_)
		sessions_.emplace_back(sid, std::make_shared<Session>());
	// buffer is full -> discard last entry;
	else
		sessions_.back() = std::make_pair(sid, std::make_shared<Session>());
	return sessions_.back().second;
}

////////////////////////////////////////////////////////////////////////////////
pcw::SessionPtr
pcw::Sessions::session(const std::string& sid) const
{
	std::lock_guard<std::mutex> lock(mutex_);
	auto i = std::find_if(begin(sessions_), end(sessions_), [&sid](const auto& p) {
		return p.first == sid;
	});
	if (i == begin(sessions_)) {
		return i->second;
	} else if (i != end(sessions_)) {
		auto j = std::prev(i);
		std::swap(*j, *i);
		assert(j->first == sid);
		return j->second;
	} else {
		return nullptr;
	}
}
