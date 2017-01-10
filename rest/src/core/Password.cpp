#include <algorithm>
#include <random>
#include <unistd.h>
#include <system_error>
#include "Password.hpp"

#ifndef PCW_PASSWORD_SEED_SIZE
#define PCW_PASSWORD_SEED_SIZE 10
#endif // PCW_PASSWORD_SEED_SIZE

#ifndef PCW_PASSWORD_ALGORITHM
#define PCW_PASSWORD_ALGORITHM "$2a$"
#endif // PCW_PASSWORD_ALGORITHM

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
bool
is_salted_hash(const std::string& salted_hash)
{
	if (salted_hash.size() <= (PCW_PASSWORD_SEED_SIZE + 4))
		return false;
	if (salted_hash.at(0) != '$')
		return false;
	if (salted_hash.at(2) != '$')
		return false;
	if (salted_hash.at(PCW_PASSWORD_SEED_SIZE + 3) != '$')
		return false;
	return true;
}

////////////////////////////////////////////////////////////////////////////////
static std::string
get_salt(const std::string& salted_hash)
{
	if (not is_salted_hash(salted_hash))
		throw std::runtime_error("(Password) Invalid password hash: " +
				salted_hash);
	return salted_hash.substr(0, PCW_PASSWORD_SEED_SIZE + 4);
}

////////////////////////////////////////////////////////////////////////////////
static std::string
get_hash(const std::string& salted_hash)
{
	if (not is_salted_hash(salted_hash))
		throw std::runtime_error("(Password) Invalid password hash: " +
				salted_hash);
	return salted_hash.substr(PCW_PASSWORD_SEED_SIZE + 4);
}


////////////////////////////////////////////////////////////////////////////////
Password
Password::make(const std::string& password)
{
	static const char chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		                    "abcdefghijklmnopqrstuvwxyz";
	std::random_device r;
	std::default_random_engine e(r());
	std::uniform_int_distribution<int> d(0, 51);
	std::string salt = PCW_PASSWORD_ALGORITHM;

	std::generate_n(std::back_inserter(salt), PCW_PASSWORD_SEED_SIZE,
			[&](){return chars[d(e)];});

	auto salted_hash = crypt(password.data(), salt.data());
	if (not salted_hash)
		throw std::system_error(errno, std::system_category(), "crypt");
	return Password(salted_hash);
}

////////////////////////////////////////////////////////////////////////////////
Password::Password(const std::string& salted_hash)
	: salt_(get_salt(salted_hash))
	, hash_(get_hash(salted_hash))
{
}

////////////////////////////////////////////////////////////////////////////////
bool
Password::authenticate(const std::string& password) const
{
	auto tmp = crypt(password.data(), salt_.data());
	return get_salt(tmp) == salt_ and get_hash(tmp) == hash_;
}

