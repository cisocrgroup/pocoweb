#ifndef pcw_Sessions_hpp__
#define pcw_Sessions_hpp__

namespace pcw {
	class User;
	using UserPtr = std::shared_ptr<User>;

	class Sessions {
	public:
		Sessions();
		bool insert(const std::string& sid, const UserPtr& user);
		UserPtr find(const std::string& sid) const;

	private:
		mutable std::vector<std::pair<std::string, UserPtr> > sessions_;
		mutable std::mutex mutex_;
	};
}

#endif // pcw_Sessions_hpp__
