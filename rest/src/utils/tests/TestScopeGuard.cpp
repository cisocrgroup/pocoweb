#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE ScopeGuardTest

#include <boost/test/unit_test.hpp>
#include <iostream>
#include "utils/ScopeGuard.hpp"

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(CallsUndoFunction)
{
	bool was_called = false;
	{
		ScopeGuard sg([&](){was_called = true;});
	}
	BOOST_CHECK(was_called);
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(DoesNotCallUndoFunctionAfterDismiss)
{
	ScopeGuard sg([](){BOOST_ERROR("Undo function was called");});
	sg.dismiss();
}
