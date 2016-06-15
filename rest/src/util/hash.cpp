#include <cassert>
#include <cstring>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <string>
#include <random>
#include <unistd.h>
#include <openssl/sha.h>
#include "hash.hpp"

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

	std::uniform_int_distribution<char> d('0', 'z');
	std::mt19937 gen(seed);
	for (size_t i = 0; i < n; ++i)
		id[i] = d(gen);
	return dohash(id.data(), id.size(), "", 0);
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
