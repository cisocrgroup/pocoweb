#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE RemoteProfilerTest

#include <boost/test/unit_test.hpp>
#include "core/BookBuilder.hpp"
#include "profiler/Profile.hpp"
#include "profiler/RemoteProfiler.hpp"
#include "utils/Base64.hpp"
#include "utils/Maybe.hpp"

using namespace pcw;

static const char* url =
    "http://langprofiler.informatik.uni-leipzig.de/axis2/services/"
    "ProfilerWebService";

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(GetLanguages) {
	RemoteProfiler profiler(nullptr, url);
	const auto ls = profiler.languages();
	BOOST_REQUIRE(ls.ok());
	const auto contains = [&ls](const std::string& l) {
		const auto i = std::find(begin(ls.get()), end(ls.get()), l);
		return i != end(ls.get());
	};
	BOOST_CHECK(contains("german"));
	BOOST_CHECK(contains("latin"));
	BOOST_CHECK(contains("greek"));
	BOOST_CHECK(not contains("english"));
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(GetProfile) {
	BookBuilder builder;
	builder.append_text("Erstä Zaile\nZwoite Zaila\nDritte Zeile");
	builder.append_text("Nöch aine Zaile\nLetzte Zaile\n");
	builder.set_language("german");
	RemoteProfiler profiler(builder.build(), url);
	const auto mprofile = profiler.profile();
	// BOOST_REQUIRE(mprofile.ok());
	const auto profile = mprofile.get();
}
