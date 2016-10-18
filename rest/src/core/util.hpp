#ifndef pcw_util_hpp__
#define pcw_util_hpp__

#include <stdexcept>

namespace pcw {
	const char* what(const std::exception& e) noexcept;
	std::string gensessionid(size_t n);
	std::string gensalt();
	std::string genhash(const std::string& salt, const std::string& str);
	bool check(const std::string& comb, const std::string& passwd);
}

#endif // pcw_util_hpp__
