#ifndef pcw_User_hpp__
#define pcw_User_hpp__

#include <memory>

namespace pcw {
	class Project;
	class User;
	using UserPtr = std::shared_ptr<User>;

	class User: public std::enable_shared_from_this<User> {
	public:
		User(std::string n,
		     std::string e,
		     std::string i,
		     int id)
			: name(std::move(n))
			, email(std::move(e))
			, institute(std::move(i))
			, id_(id)
		{}
		bool has_permission(const Project& project) const noexcept;
		int id() const noexcept {return id_;}
		
		const std::string name;
		std::string email, institute;

	private:
		const int id_;
	};
	std::ostream& operator<<(std::ostream& os, const User& user);
}

#endif // pcw_User_hpp__
