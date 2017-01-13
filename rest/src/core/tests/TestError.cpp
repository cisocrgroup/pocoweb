#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE ErrorTest

#include <boost/test/unit_test.hpp>
#include <iostream>
#include "core/Error.hpp"

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(ThrowError)
{
	BOOST_CHECK_THROW(THROW(Error), std::exception);
	BOOST_CHECK_THROW(THROW(Error), Error);
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(ThrowNotImplemented)
{
	BOOST_CHECK_THROW(THROW(NotImplemented), Error);
	BOOST_CHECK_THROW(THROW(NotImplemented), NotImplemented);
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(ThrowBadRequest)
{
	BOOST_CHECK_THROW(THROW(BadRequest), Error);
	BOOST_CHECK_THROW(THROW(BadRequest), BadRequest);
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(ErrorCode)
{
	try {
		THROW(Error);
	} catch (const Error& error) {
		BOOST_CHECK_EQUAL(error.code(), 500);
	} catch (...) {
		BOOST_FAIL("Invalid catch(...)");
	}
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(NotImplementedCode)
{
	try {
		THROW(NotImplemented);
	} catch (const Error& error) {
		BOOST_CHECK_EQUAL(error.code(), 501);
	} catch (...) {
		BOOST_FAIL("Invalid catch(...)");
	}
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(BadRequestCode)
{
	try {
		THROW(BadRequest);
	} catch (const Error& error) {
		BOOST_CHECK_EQUAL(error.code(), 400);
	} catch (...) {
		BOOST_FAIL("Invalid catch(...)");
	}
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(ErrorMessage)
{
	try {
		THROW(Error, "Error Message: ", 1, 2, 3);
	} catch (const Error& error) {
		BOOST_CHECK_EQUAL(
			error.what(),
			"rest/src/core/tests/TestError.cpp:71: Error Message: 123"
		);
	} catch (...) {
		BOOST_FAIL("Invalid catch(...)");
	}
}

