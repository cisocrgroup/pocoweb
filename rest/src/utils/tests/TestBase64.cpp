#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE Base64Test

#include <boost/test/unit_test.hpp>
#include <iostream>
#include "utils/Base64.hpp"

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(Encode) {
	for (const auto str : {"abc", "foo-bar", "abc\ndef\n", " a b c d e "}) {
		const auto b64 = base64::encode(str);
		BOOST_CHECK_EQUAL(base64::decode(b64), str);
	}
}
