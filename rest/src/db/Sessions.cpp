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
bool
pcw::Sessions::insert(const std::string& sid, const UserPtr& user)
{
	std::lock_guard<std::mutex> lock(mutex_);
	auto i = std::find_if(begin(sessions_), end(sessions_), [&sid](const auto& p) {
		return p.first == sid;
	});

	// non unique session id
	if (i != end(sessions_))
		return false;

	// buffer is not full yet
	if (sessions_.size() < n_)
		sessions_.emplace_back(sid, std::make_shared<Session>(user));
	// buffer is full -> discard last entry;
	else
		sessions_.back() = std::make_pair(sid, std::make_shared<Session>(user));
	return true;
}

////////////////////////////////////////////////////////////////////////////////
pcw::SessionPtr
pcw::Sessions::find(const std::string& sid) const
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
