#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE UtilsTest

#include <boost/test/unit_test.hpp>
#include <iostream>
#include "core/util.hpp"

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(StringToLower) {
	std::string test = "ALLUPPER";
	to_lower(test);
	BOOST_CHECK_EQUAL(test, "allupper");
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(StringToUpper) {
	std::string test = "alllower";
	to_upper(test);
	BOOST_CHECK_EQUAL(test, "ALLLOWER");
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(WstringToLower) {
	std::wstring test = L"ALLUPPER";
	to_lower(test);
	BOOST_CHECK(test == L"allupper");
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(WstringToUpper) {
	std::wstring test = L"alllower";
	to_upper(test);
	BOOST_CHECK(test == L"ALLLOWER");
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(CapitalizationStringAllLower) {
	BOOST_CHECK(get_capitalization("alllower") ==
		    Capitalization::LowerCase);
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(CapitalizationStringFirstUpper) {
	BOOST_CHECK(get_capitalization("Firstupper") ==
		    Capitalization::Capitalized);
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(CapitalizationStringAllUpper) {
	BOOST_CHECK(get_capitalization("ALLUPPER") ==
		    Capitalization::TitleCase);
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(CapitalizationWstringgAllLower) {
	BOOST_CHECK(get_capitalization(L"alllower") ==
		    Capitalization::LowerCase);
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(CapitalizationWstringgFirstUpper) {
	BOOST_CHECK(get_capitalization(L"Firstupper") ==
		    Capitalization::Capitalized);
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(CapitalizationWstringgAllUpper) {
	BOOST_CHECK(get_capitalization(L"ALLUPPER") ==
		    Capitalization::TitleCase);
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(ApplyCapitalizationLowerCaseToString) {
	std::string test = "a VERy weIRD STRINg";
	apply_capitalization(Capitalization::LowerCase, test);
	BOOST_CHECK_EQUAL(test, "a very weird string");
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(ApplyCapitalizationTitleCaseToString) {
	std::string test = "a VERy weIRD STRINg";
	apply_capitalization(Capitalization::TitleCase, test);
	BOOST_CHECK_EQUAL(test, "A VERY WEIRD STRING");
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(ApplyCapitalizationCapitalizedToString) {
	std::string test = "a VERy weIRD STRINg";
	apply_capitalization(Capitalization::Capitalized, test);
	BOOST_CHECK_EQUAL(test, "A very weird string");
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(ApplyCapitalizationLowerCaseToWstring) {
	std::wstring test = L"a VERy weIRD STRINg";
	apply_capitalization(Capitalization::LowerCase, test);
	BOOST_CHECK(test == L"a very weird string");
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(ApplyCapitalizationTitleCaseToWstring) {
	std::wstring test = L"a VERy weIRD STRINg";
	apply_capitalization(Capitalization::TitleCase, test);
	BOOST_CHECK(test == L"A VERY WEIRD STRING");
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(ApplyCapitalizationCapitalizedToWstring) {
	std::wstring test = L"a VERy weIRD STRINg";
	apply_capitalization(Capitalization::Capitalized, test);
	BOOST_CHECK(test == L"A very weird string");
}
