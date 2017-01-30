#ifndef pcw_SessionStore_hpp__
#define pcw_SessionStore_hpp__

#include <memory>
#include <vector>

namespace pcw {
	class Session;
	using SessionSptr = std::shared_ptr<Session>;
	class SessionStore;
	using SessionStoreSptr = std::shared_ptr<SessionStore>;

	class SessionStore {
	public:
		SessionStore(size_t n): sessions_(n) {}
		size_t size() const noexcept {return sessions_.size();}

	private:
		using Sessions = std::vector<SessionSptr>;
		Sessions sessions_;
	};
}

#endif // pcw_SessionStore_hpp__
