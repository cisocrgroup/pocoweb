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
	using ConstUserPtr = std::shared_ptr<const User>;
	using ConnectionPtr = std::unique_ptr<sql::Connection>;
	using BookPtr = std::shared_ptr<Book>;
	class Sessions;
	
	struct Session {
		Session(ConstUserPtr u, ConnectionPtr c, std::string s)
			: user(std::move(u))
			, connection(std::move(c))
			, sid(std::move(s))
			, current_book()
			, mutex()
		{}
		const ConstUserPtr user;
		const ConnectionPtr connection;
		const std::string sid;

		BookPtr current_book;
		std::mutex mutex;
	};

	using SessionPtr = std::shared_ptr<Session>;

	class Sessions {
	public:
		Sessions(const Config& config);
		SessionPtr new_session(const User& user, ConnectionPtr connection);
		SessionPtr find_session(const std::string& sid) const;

	private:
		SessionPtr insert(SessionPtr session);

		mutable std::vector<SessionPtr> sessions_;
		mutable std::mutex mutex_;
		const size_t n_;
	};
}

#endif // pcw_Sessions_hpp__
