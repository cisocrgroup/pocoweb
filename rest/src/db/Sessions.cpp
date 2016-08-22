#include <algorithm>
#include <cassert>
#include <thread>
#include <memory>
#include <mutex>
#include <vector>
#include <json.hpp>
#include "User.hpp"
#include "Sessions.hpp"

////////////////////////////////////////////////////////////////////////////////
pcw::Sessions::Sessions()
	: sessions_()
	, mutex_()
{
	sessions_.reserve(100);
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
	if (sessions_.size() < 100)
		sessions_.emplace_back(sid, user);
	// buffer is full -> discard last entry;
	else
		sessions_.back() = std::make_pair(sid, user);
	return true;
}

////////////////////////////////////////////////////////////////////////////////
boost::optional<pcw::UserPtr>
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
		return {};
	}
}
