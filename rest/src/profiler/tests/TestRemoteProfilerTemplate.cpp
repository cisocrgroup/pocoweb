#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE RemoteProfilerTemplateTest

#include <boost/test/unit_test.hpp>
#include "profiler/RemoteProfilerTemplate.hpp"
#include "utils/Base64.hpp"

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(SetLanguage) {
	const auto tmpl =
	    RemoteProfilerTemplate().set_language("language").get();
	BOOST_CHECK(
	    tmpl.find("<ns1:configuration>language</ns1:configuration>") !=
	    std::string::npos);
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(SetUserid) {
	const auto tmpl = RemoteProfilerTemplate().set_userid("userid").get();
	BOOST_CHECK(tmpl.find("<ns1:userid>userid</ns1:userid>") !=
		    std::string::npos);
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(SetFileName) {
	const auto tmpl =
	    RemoteProfilerTemplate().set_filename("filename").get();
	BOOST_CHECK(tmpl.find("<ns1:fileName>filename</ns1:fileName>") !=
		    std::string::npos);
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(SetData) {
	const auto tmpl = RemoteProfilerTemplate().set_data("data").get();
	const auto b64 = base64::encode("data");
	const auto size = std::to_string(b64.size());
	BOOST_CHECK(tmpl.find("<ns1:binaryData>" + b64 + "</ns1:binaryData>") !=
		    std::string::npos);
	BOOST_CHECK(tmpl.find("<ns1:doc_in_size>" + size +
			      "</ns1:doc_in_size>") != std::string::npos);
}
