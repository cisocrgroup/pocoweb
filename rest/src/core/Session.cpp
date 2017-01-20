#include "util.hpp"
#include "User.hpp"
#include "Session.hpp"
#include "Cache.hpp"
#include "AppCache.hpp"

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
Session::Session(const User& user, AppCacheSptr cache)
	: sid_(gensessionid(16))
	, user_(user.shared_from_this())
	, cache_(std::move(cache))
	, mutex_()
{}

////////////////////////////////////////////////////////////////////////////////
void
Session::cache(BookView& view) const
{
	if (cache_) {
		cache_->project.put(view.shared_from_this());
	}
}

////////////////////////////////////////////////////////////////////////////////
void
Session::cache(User& user) const
{
	if (cache_) {
		cache_->user.put(user.shared_from_this());
	}
}
