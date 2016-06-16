#ifndef pcw_DbTableUsers_hpp__
#define pcw_DbTableUsers_hpp__

namespace pcw {
	class User;
	using UserPtr = std::shared_ptr<User>;

	class DbTableUsers {
	public:
		DbTableUsers(ConnectionPtr conn);
		UserPtr findUserByEmail(const std::string& email) const;
		UserPtr findUserByName(const std::string& name) const;
		UserPtr findUserByNameOrEmail(const std::string& what) const;
		UserPtr createUser(const std::string& name,
				   const std::string& email,
				   const std::string& institute,
				   const std::string& password) const;
		template<class F> std::vector<UserPtr> findUsers(F f) const;
		bool authenticate(const User& user, const std::string& passwd) const;

	private:
		static bool authenticate(const std::string& hash,
					 const std::string& passwd);
		const ConnectionPtr conn_;
	};
}

////////////////////////////////////////////////////////////////////////////////
template<class F>
std::vector<pcw::UserPtr>
pcw::DbTableUsers::findUsers(F f) const
{
	assert(conn_);
	StatementPtr s(conn_->createStatement());
	assert(s);
	ResultSetPtr res(s->executeQuery("select * from users"));
	assert(res);
	std::vector<UserPtr> users;
	users.reserve(res->rowsCount());
	while (res->next()) {
		auto user = User::create(*res);
		if (user and f(*user))
			users.push_back(std::move(user));
	}
	return users;
}

#endif // pcw_DbTableUsers_hpp__
