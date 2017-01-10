#ifndef pcw_Password_hpp__
#define pcw_Password_hpp__

#include <string>

namespace pcw {
	class Password {
	public:
		static Password make(const std::string& password);
		Password(const std::string& salted_hash);

		bool authenticate(const std::string& password) const;
		std::string str() const {return salt_ + hash_;}

	private:
		const std::string salt_, hash_;
	};
}

#endif // pcw_Password_hpp__
