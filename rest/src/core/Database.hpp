#ifndef pcw_Database_hpp__
#define pcw_Database_hpp__

namespace sql {
	class Connection;
}

namespace pcw {
	class Session;
	using SessionPtr = std::shared_ptr<Session>;
	class Config;
	using ConfigPtr = std::shared_ptr<Config>;
	class User;
	using UserPtr = std::shared_ptr<User>;

	class Database {
	public:
		Database(SessionPtr session, ConfigPtr config);
		
		UserPtr insert_user(const std::string& name, const std::string& pass) const;
		UserPtr authenticate(const std::string& name, const std::string& pass) const;

	private:
		sql::Connection* connection() const;

		const SessionPtr session_;
		const ConfigPtr config_;
	};
}

#endif // pcw_Database_hpp__
