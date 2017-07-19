#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE LineBuilderTest

#include <boost/test/unit_test.hpp>
#include <iostream>
#include "core/Line.hpp"
#include "core/LineBuilder.hpp"
#include "core/Page.hpp"

using namespace pcw;

struct LineBuilderFixture {
	LineBuilder builder;
};

////////////////////////////////////////////////////////////////////////////////
BOOST_FIXTURE_TEST_SUITE(LineBuilder, LineBuilderFixture)

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(ResetTest) {
	auto first = builder.build();
	builder.reset();
	auto second = builder.build();
	BOOST_CHECK(first != second);
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(ImgTest) {
	builder.set_image_path("image");
	BOOST_CHECK_EQUAL(builder.build()->img, "image");
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(BoxTest) {
	builder.set_box(Box{1, 2, 3, 4});
	BOOST_CHECK_EQUAL(builder.build()->box, Box(1, 2, 3, 4));
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(AppendWcharsTest) {
	builder.append(L'ф', 1, 1.0);
	builder.append(L'о', 2, 1.0);
	builder.append(L'н', 3, 1.0);
	builder.append(L'д', 4, 1.0);
	builder.append(L'ы', 5, 1.0);
	BOOST_CHECK_EQUAL(builder.build()->ocr(), "фонды");
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(AppendWstrTest) {
	builder.append(L"фонды", 5, 1.0);
	BOOST_CHECK_EQUAL(builder.build()->ocr(), "фонды");
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(AppendU8strTest) {
	builder.append("фонды", 5, 1.0);
	BOOST_CHECK_EQUAL(builder.build()->ocr(), "фонды");
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_SUITE_END()
