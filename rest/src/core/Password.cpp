#include <iostream>
#include <algorithm>
#include <random>
#include <unistd.h>
#include <crypt.h>
#include <system_error>
#include "utils/Error.hpp"
#include "Password.hpp"

#ifndef PCW_PASSWORD_SEED_SIZE
#define PCW_PASSWORD_SEED_SIZE 2
#endif // PCW_PASSWORD_SEED_SIZE

#ifndef PCW_PASSWORD_ALGORITHM
#define PCW_PASSWORD_ALGORITHM "$6$"
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
		THROW(Error, "invalid password hash: ", salted_hash);
	return salted_hash.substr(0, PCW_PASSWORD_SEED_SIZE + 4);
}

////////////////////////////////////////////////////////////////////////////////
static std::string
get_hash(const std::string& salted_hash)
{
	if (not is_salted_hash(salted_hash))
		THROW(Error, "invalid password hash: ", salted_hash);
	return salted_hash.substr(PCW_PASSWORD_SEED_SIZE + 4);
}


////////////////////////////////////////////////////////////////////////////////
Password
Password::make(const std::string& password)
{
	static const char salt_chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		                         "abcdefghijklmnopqrstuvwxyz"
				         "0123456789./";
	std::random_device r;
	std::default_random_engine e(r());
	std::uniform_int_distribution<int> d(0, 63);

	std::string salt = PCW_PASSWORD_ALGORITHM;
	std::generate_n(std::back_inserter(salt), PCW_PASSWORD_SEED_SIZE,
			[&](){return salt_chars[d(e)];});
	salt += '$';

	crypt_data data;
	data.initialized = 0;
	auto salted_hash = crypt_r(password.data(), salt.data(), &data);
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
	crypt_data data;
	data.initialized = 0;
	auto tmp = crypt_r(password.data(), salt_.data(), &data);
	return get_salt(tmp) == salt_ and get_hash(tmp) == hash_;
}

