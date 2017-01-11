#ifndef pcw_NewDatabase_hpp__
#define pcw_NewDatabase_hpp__

#include <memory>

namespace pcw {
	class User;
	using UserSptr = std::shared_ptr<User>;

	template<class Db>
	UserSptr
	create_user(Db& db, const std::string& user,
			const std::string& pw,
			const std::string& email = "",
			const std::string& inst = "");


}

////////////////////////////////////////////////////////////////////////////////
template<class Db>
pcw::UserSptr
pcw::create_user(Db& db, const std::string& name, const std::string& pw,
			const std::string& email, const std::string& inst)
{
	auto password = Password::make(pw);
	tables::Users users;
	auto stmt = insert_into(users).set(
		users.name = name,
		users.email = email,
		users.institute = inst,
		users.passwd = password.str()
	);
	auto id = db(stmt);
	return std::make_shared<User>(name, email, inst, id);
}

#endif // pcw_NewDatabase_hpp__
