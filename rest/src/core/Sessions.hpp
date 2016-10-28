#ifndef pcw_Sessions_hpp__
#define pcw_Sessions_hpp__

#include <vector>
#include <mutex>
#include <memory>

namespace sql {
	class Connection;
}

namespace pcw {
	class Config;
	class User;
	class Book;
	using UserPtr = std::shared_ptr<User>;
	using ConnectionPtr = std::unique_ptr<sql::Connection>;
	using BookPtr = std::shared_ptr<Book>;
	class Sessions;
	
	struct Session {
		Session(std::string s)
			: sid(std::move(s))
			, user()
			, connection()
			, mutex()
		{}
		const std::string sid;
		UserPtr user;
		ConnectionPtr connection;
		std::mutex mutex;
	};

	using SessionPtr = std::shared_ptr<Session>;

	class Sessions {
	public:
		Sessions(const Config& config);
		SessionPtr new_session();
		SessionPtr find_session(const std::string& sid) const;

	private:
		SessionPtr insert(SessionPtr session);

		mutable std::vector<SessionPtr> sessions_;
		mutable std::mutex mutex_;
		const size_t n_;
	};
}

#endif // pcw_Sessions_hpp__
