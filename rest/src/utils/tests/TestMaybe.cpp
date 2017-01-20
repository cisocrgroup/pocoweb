#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE MaybeTest

#include <boost/test/unit_test.hpp>
#include <iostream>
#include "utils/Maybe.hpp"

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(Throw)
{
	auto maybe = Maybe<int>::from_lambda([]()->int {
		throw std::runtime_error("test-exception");
	});
	BOOST_CHECK(not maybe.ok());
	BOOST_CHECK_THROW(maybe.get(), std::runtime_error);
	BOOST_CHECK(maybe.has<std::runtime_error>());
	BOOST_CHECK(maybe.has<std::exception>());
	BOOST_CHECK(not maybe.has<std::logic_error>());
	BOOST_CHECK_EQUAL(maybe.what<std::runtime_error>(), "test-exception");
	BOOST_CHECK_EQUAL(maybe.what<std::exception>(), "test-exception");
	BOOST_CHECK(maybe.what<std::logic_error>() == nullptr);
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(Nothrow)
{
	Maybe<int> maybe(15);
	BOOST_CHECK(maybe.ok());
	BOOST_CHECK(not maybe.has<std::exception>());
	BOOST_CHECK(maybe.what<std::exception>() == nullptr);
	BOOST_CHECK_NO_THROW(maybe.get());
	BOOST_CHECK_EQUAL(maybe.get(), 15);
}
