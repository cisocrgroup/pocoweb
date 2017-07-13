#include "Session.hpp"
#include <crow/http_request.h>
#include <crow/http_response.h>
#include <crow/logging.h>
#include <regex>
#include "AppCache.hpp"
#include "Cache.hpp"
#include "SessionDirectory.hpp"
#include "User.hpp"
#include "util.hpp"

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
Session::Session(const User& user, AppCacheSptr cache)
    : sid_(gensessionid(SESSION_ID_LENGTH)),
      user_(user.shared_from_this()),
      dir_(std::make_unique<SessionDirectory>(gensessionid(SESSION_ID_LENGTH))),
      cache_(std::move(cache)),
      mutex_(),
      project_(),
      page_() {}

////////////////////////////////////////////////////////////////////////////////
void Session::cache(Project& view) const {
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
void Session::cache(User& user) const {
	if (cache_) {
		cache_->users.put(user.shared_from_this());
	}
}

////////////////////////////////////////////////////////////////////////////////
void Session::uncache_project(int pid) const {
	if (cache_) cache_->projects.del(pid);
}
