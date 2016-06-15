#ifndef pcw_User_hpp__
#define pcw_User_hpp__

namespace sql {
	class ResultSet;
}

namespace pcw {
	class User;
	using UserPtr = std::shared_ptr<User>;

	class User: public std::enable_shared_from_this<User> {
	public:
		User(std::string n,
		     std::string e,
		     std::string i,
		     int iid);
		static UserPtr create(const sql::ResultSet& res);
		bool auth(const std::string& passwd);

		const std::string name, email, institute;
		const int id;
	};
	std::ostream& operator<<(std::ostream& os, const User& user);
}

#endif // pcw_User_hpp__
