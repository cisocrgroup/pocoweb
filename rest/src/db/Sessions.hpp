#ifndef pcw_Sessions_hpp__
#define pcw_Sessions_hpp__

#include <boost/optional.hpp>
#include <vector>
#include <mutex>

namespace pcw {
	class User;
	using UserPtr = std::shared_ptr<User>;
	
	struct Session {
		UserPtr user;
	};

	class Sessions {
	public:
		Sessions();
		bool insert(const std::string& sid, const UserPtr& user);
		boost::optional<Session> find(const std::string& sid) const;

	private:
		mutable std::vector<std::pair<std::string, Session>> sessions_;
		mutable std::mutex mutex_;
	};
}

#endif // pcw_Sessions_hpp__
