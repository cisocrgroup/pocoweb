#ifndef pcw_User_hpp__
#define pcw_User_hpp__

#include <memory>
#include "Password.hpp"

namespace pcw {
class Project;
class User;
using UserPtr = std::shared_ptr<User>;
using UserSptr = std::shared_ptr<User>;

class User : public std::enable_shared_from_this<User> {
       public:
	User(std::string n, Password pass, std::string e, std::string i, int id,
	     bool admin = false);
	User(std::string n, const std::string& p, std::string e, std::string i,
	     int id, bool admin = false);
	int id() const noexcept { return id_; }
	bool admin() const noexcept { return admin_; }

	std::string name;
	Password password;
	std::string email, institute;

       private:
	const int id_;
	const bool admin_;
};
std::ostream& operator<<(std::ostream& os, const User& user);
}

#endif  // pcw_User_hpp__
