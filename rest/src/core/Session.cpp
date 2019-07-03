#include "Session.hpp"
#include "AppCache.hpp"
#include "Cache.hpp"
#include "SessionDirectory.hpp"
#include <crow/http_request.h>
#include <crow/http_response.h>
#include <crow/logging.h>
#include <regex>

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
Session::Session(int user, const Config &config, AppCacheSptr cache)
    : sid_(std::to_string(user)) // gensessionid(SESSION_ID_LENGTH))
      ,
      user_(user), dir_(std::make_unique<SessionDirectory>(
                       gensessionid(SESSION_ID_LENGTH), config)),
      cache_(std::move(cache)), mutex_(), project_(), page_() {}

////////////////////////////////////////////////////////////////////////////////
void Session::cache(Project &view) const {
  if (cache_) {
    if (view.is_book()) {
      auto book = std::dynamic_pointer_cast<Book>(view.shared_from_this());
      cache_->books.put(book);
    } else {
      cache_->projects.put(view.shared_from_this());
    }
  }
}

////////////////////////////////////////////////////////////////////////////////
void Session::set_user(int user) noexcept { user_ = user; }

////////////////////////////////////////////////////////////////////////////////
void Session::uncache_project(int pid) const {
  // remove cached project and page (to be sure) from session
  if (project_ and project_->id() == pid) {
    page_ = nullptr;
    project_ = nullptr;
  }
  if (cache_) {
    cache_->projects.del(pid);
  }
}
