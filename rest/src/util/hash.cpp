#include <sstream>
#include <iomanip>
#include <string>
#include <random>
#include <openssl/sha.h>
#include "hash.hpp"

////////////////////////////////////////////////////////////////////////////////
std::string
pcw::salt()
{
	std::uniform_int_distribution<unsigned int> d;
	std::mt19937 gen;
	const auto n = d(gen);

	std::stringstream ios;
	ios << std::setw(sizeof(unsigned int))
	    << std::setfill('0')
	    << std::hex
	    << n;
	return ios.str();
}

////////////////////////////////////////////////////////////////////////////////
static std::string
dohash(const char *salt, size_t saltn, const char *str, size_t strn)
{
	// ERROR CHECKING!!
	unsigned char hash[SHA256_DIGEST_LENGTH];
	SHA256_CTX sha256;
	SHA256_Init(&sha256);
	SHA256_Update(&sha256, salt, saltn);
	SHA256_Update(&sha256, str, strn);
	SHA256_Final(hash, &sha256);

	std::stringstream ios;
	for (auto i = 0; i < SHA256_DIGEST_LENGTH; ++i)
		ios << std::setw(2)
		    << std::setfill('0')
		    << std::hex
		    << hash[i];
	return ios.str();
}

////////////////////////////////////////////////////////////////////////////////
std::string
pcw::hash(const std::string& salt, const std::string& str)
{
	return dohash(salt.data(), salt.size(), str.data(), str.size());
}
