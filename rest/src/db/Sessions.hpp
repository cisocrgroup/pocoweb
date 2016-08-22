#ifndef pcw_Sessions_hpp__
#define pcw_Sessions_hpp__

#include <vector>
#include <mutex>

namespace pcw {
	class Config;
	class User;
	using UserPtr = std::shared_ptr<User>;
	
	struct Session {
		Session(UserPtr u)
			: user(std::move(u))
		{}
		UserPtr user;
	};
	using SessionPtr = std::shared_ptr<Session>;

	class Sessions {
	public:
		Sessions(const Config& config);
		bool insert(const std::string& sid, const UserPtr& user);
		SessionPtr find(const std::string& sid) const;

	private:
		mutable std::vector<std::pair<std::string, SessionPtr>> sessions_;
		mutable std::mutex mutex_;
		const size_t n_;
	};
}

#endif // pcw_Sessions_hpp__
