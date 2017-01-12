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
	template<class Db>
	UserSptr
	login_user(Db& db, const std::string& user, const std::string& pw);

	template<class Db>
	void
	update_user(Db& db, const User& user);

}

////////////////////////////////////////////////////////////////////////////////
template<class Db>
pcw::UserSptr
pcw::create_user(Db& db, const std::string& name, const std::string& pw,
			const std::string& email, const std::string& inst)
{
	using namespace sqlpp;
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

////////////////////////////////////////////////////////////////////////////////
template<class Db>
pcw::UserSptr
pcw::login_user(Db& db, const std::string& name, const std::string& pw)
{
	using namespace sqlpp;
	tables::Users users;
	auto stmt = select(all_of(users)).from(users).where(users.name == name);
	auto res = db(stmt);
	if (not res.empty()) {
		Password password(res.front().passwd);
		if (password.authenticate(pw)) {
			return std::make_shared<User>(
				res.front().name,
				res.front().email,
				res.front().institute,
				res.front().userid
			);
		}
	}
	return nullptr;
}

////////////////////////////////////////////////////////////////////////////////
template<class Db>
void
pcw::update_user(Db& db, const User& user)
{
	using namespace sqlpp;
	tables::Users users;
	// do not change user's name and id
	auto stmt = update(users).set(
		users.email = user.email,
		users.institute = user.institute
	).where(users.userid == user.id());
	db(stmt);
}

#endif // pcw_NewDatabase_hpp__
