#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE TestPassword

#include <boost/test/unit_test.hpp>
#include <iostream>
#include "core/Password.hpp"

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(Authenticate)
{
	std::string pw1 = "first-test-password";
	std::string pw2 = "second-test-password";
	auto password =  Password::make(pw1);
	BOOST_CHECK(password.authenticate(pw1));
	BOOST_CHECK(not password.authenticate(pw2));
}

