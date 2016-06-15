#ifndef pcw_hash_hpp__
#define pcw_hash_hpp__

namespace pcw {
	std::string gensessionid(size_t n);
	std::string gensalt();
	std::string genhash(const std::string& salt, const std::string& str);
	bool check(const std::string& comb, const std::string& passwd);
}

#endif // pcw_hash_hpp__
