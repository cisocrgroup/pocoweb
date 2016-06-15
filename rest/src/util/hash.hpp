#ifndef pcw_hash_hpp__
#define pcw_hash_hpp__

namespace pcw {
	std::string salt();
	std::string hash(const std::string& salt, const std::string& str);
}

#endif // pcw_hash_hpp__
