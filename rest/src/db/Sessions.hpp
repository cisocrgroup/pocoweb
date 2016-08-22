#ifndef pcw_Sessions_hpp__
#define pcw_Sessions_hpp__

#include <vector>
#include <mutex>

namespace sql {
	class Connection;
}

namespace pcw {
	class Config;
	class User;
	using UserPtr = std::shared_ptr<User>;
	using ConnectionPtr = std::shared_ptr<sql::Connection>;
	
	struct Session {
		UserPtr user;
		ConnectionPtr connection;
		std::string sid;
	};
	using SessionPtr = std::shared_ptr<Session>;

	class Sessions {
	public:
		Sessions(const Config& config);
		SessionPtr new_session(const std::string& sid);
		SessionPtr session(const std::string& sid) const;

	private:
		mutable std::vector<std::pair<std::string, SessionPtr>> sessions_;
		mutable std::mutex mutex_;
		const size_t n_;
	};
}

#endif // pcw_Sessions_hpp__
