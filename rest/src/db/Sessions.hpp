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
	
	struct Session {
		UserPtr user;
		ConnectionPtr connection;
		BookPtr current_book;
		std::string sid;
		std::mutex mutex;
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
