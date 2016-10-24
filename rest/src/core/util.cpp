#include <cstring>
#include <cassert>
#include <cstring>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <string>
#include <random>
#include <unistd.h>
#include <regex>
#include <openssl/sha.h>
#include "util.hpp"

#ifndef PCW_WHAT_LEN
#define PCW_WHAT_LEN 1024
#endif // PCW_WHAT_LEN

// using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
const char*
pcw::what(const std::exception& e) noexcept
{
	static char buffer[PCW_WHAT_LEN];
	return strncpy(buffer, e.what(), PCW_WHAT_LEN);
}

////////////////////////////////////////////////////////////////////////////////
// http://stackoverflow.com/a/323302
// http://www.concentric.net/~Ttwang/tech/inthash.htm
static unsigned long
mix(unsigned long a, unsigned long b, unsigned long c)
{
    a=a-b;  a=a-c;  a=a^(c >> 13);
    b=b-c;  b=b-a;  b=b^(a << 8);
    c=c-a;  c=c-b;  c=c^(b >> 13);
    a=a-b;  a=a-c;  a=a^(c >> 12);
    b=b-c;  b=b-a;  b=b^(a << 16);
    c=c-a;  c=c-b;  c=c^(b >> 5);
    a=a-b;  a=a-c;  a=a^(c >> 3);
    b=b-c;  b=b-a;  b=b^(a << 10);
    c=c-a;  c=c-b;  c=c^(b >> 15);
    return c;
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
	for (auto i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
		ios << std::setw(2)
		    << std::setfill('0')
		    << std::hex
		    << static_cast<int>(hash[i]);
	}
	return ios.str();
}

////////////////////////////////////////////////////////////////////////////////
std::string
pcw::gensessionid(size_t n)
{
	unsigned long seed = mix(clock(), time(NULL), getpid());
	std::string id(n, 0);

	std::uniform_int_distribution<char> d('a', 'z');
	std::mt19937 gen(seed);
	for (auto& c: id)
		c = d(gen);
	return id;
}

////////////////////////////////////////////////////////////////////////////////
std::string
pcw::gensalt()
{
	unsigned long seed = mix(clock(), time(NULL), getpid());
	std::uniform_int_distribution<unsigned int> d;
	std::mt19937 gen(seed);
	const auto n = d(gen);

	std::stringstream ios;
	ios << std::setw(sizeof(unsigned int))
	    << std::setfill('0')
	    << std::hex
	    << n;
	return ios.str();
}

////////////////////////////////////////////////////////////////////////////////
std::string
pcw::genhash(const std::string& salt, const std::string& str)
{
	return dohash(salt.data(), salt.size(), str.data(), str.size());
}

////////////////////////////////////////////////////////////////////////////////
bool
pcw::check(const std::string& comb, const std::string& passwd)
{
	auto ptr = std::find(comb.data(), comb.data() + comb.size(), '$');
	assert(ptr != comb.data() + comb.size());
	const auto n = std::distance(comb.data(), ptr);
	auto hash = dohash(comb.data(), n, passwd.data(), passwd.size());
	return strcmp(ptr + 1, hash.data()) == 0;
}

////////////////////////////////////////////////////////////////////////////////
std::string 
pcw::fix_windows_path(std::string path)
{
	const std::regex drive{R"(^[a-zA-Z]:\\+)"};
	path = std::regex_replace(path, drive, "/");
	std::replace(begin(path), end(path),  '\\', '/');
	return path;
}
