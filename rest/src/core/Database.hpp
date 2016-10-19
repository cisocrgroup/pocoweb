#ifndef pcw_Database_hpp__
#define pcw_Database_hpp__

#include <boost/optional.hpp>
#include <memory>
#include "ScopeGuard.hpp"

namespace sql {
	class Connection;
	class ResultSet;
}

namespace pcw {
	class Session;
	using SessionPtr = std::shared_ptr<Session>;
	class Config;
	using ConfigPtr = std::shared_ptr<Config>;
	class User;
	using UserPtr = std::shared_ptr<User>;
	using ResultSetPtr = std::unique_ptr<sql::ResultSet>;

	class Database {
	public:
		Database(SessionPtr session, ConfigPtr config);
		
		void set_autocommit(bool ac = true);
		void commit();

		UserPtr insert_user(const std::string& name, const std::string& pass) const;
		UserPtr authenticate(const std::string& name, const std::string& pass) const;
		UserPtr select_user(const std::string& name) const;
		void update_user(const User& user) const;
		void delete_user(const std::string& name) const;

	private:
		static UserPtr get_user_from_result_set(ResultSetPtr res);
		sql::Connection* connection() const;

		boost::optional<ScopeGuard> scope_guard_;
		const SessionPtr session_;
		const ConfigPtr config_;
	};
}

#endif // pcw_Database_hpp__
