#ifndef pcw_User_hpp__
#define pcw_User_hpp__

namespace pcw {
	class User: public std::enable_shared_from_this<User> {
	public:
		User(std::string n,
		     std::string e,
		     std::string i,
		     int iid);
		const std::string name, email, institute;
		const int id;
	};
	using UserPtr = std::shared_ptr<User>;
	std::ostream& operator<<(std::ostream& os, const User& user);
}

#endif // pcw_User_hpp__
