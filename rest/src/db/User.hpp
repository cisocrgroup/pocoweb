#ifndef pcw_User_hpp__
#define pcw_User_hpp__

namespace sql {
	class ResultSet;
	class Connection;
}

namespace pcw {
	class Json;
	class User;
	using UserPtr = std::shared_ptr<User>;

	class User: public std::enable_shared_from_this<User> {
	public:
		User(std::string n,
		     std::string e,
		     std::string i,
		     int iid);
		static UserPtr create(sql::Connection& conn, const std::string& n);
		static UserPtr create(const sql::ResultSet& res);
		void store(sql::Connection& conn, const std::string& passwd, bool active) const;
		bool authenticate(sql::Connection& conn, const std::string& passwd) const;
		nlohmann::json json() const;
		

		const std::string name, email, institute;
		const int id;
	};
	std::ostream& operator<<(std::ostream& os, const User& user);
}

#endif // pcw_User_hpp__
