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
	, expiration_date_()
	, mutex_()
{}

////////////////////////////////////////////////////////////////////////////////
bool
Session::has_expired() const noexcept
{
	Lock lock(mutex_);
	const auto now = std::chrono::system_clock::now();
	return expiration_date_ <= now;
}

////////////////////////////////////////////////////////////////////////////////
void
Session::cache(BookView& view) const
{
	if (cache_) {
		if (view.is_book()) {
			auto book = std::dynamic_pointer_cast<Book>(
					view.shared_from_this());
			cache_->books.put(book);
		} else {
			cache_->projects.put(view.shared_from_this());
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
void
Session::cache(User& user) const
{
	if (cache_) {
		cache_->users.put(user.shared_from_this());
	}
}
