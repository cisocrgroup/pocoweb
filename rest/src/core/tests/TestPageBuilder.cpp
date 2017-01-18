#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE PageBuilderTest

#include <boost/test/unit_test.hpp>
#include <iostream>
#include "core/LineBuilder.hpp"
#include "core/PageBuilder.hpp"
#include "core/Page.hpp"
#include "core/Page.hpp"

using namespace pcw;

struct PageBuilderFixture {
	PageBuilder pbuilder;
};

////////////////////////////////////////////////////////////////////////////////
BOOST_FIXTURE_TEST_SUITE(PageBuilder, PageBuilderFixture)

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(ResetTest)
{
	auto first = pbuilder.build();
	pbuilder.reset();
	auto second = pbuilder.build();
	BOOST_CHECK(first != second);
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(ImageTest)
{
	pbuilder.set_image_path("image");
	BOOST_CHECK_EQUAL(pbuilder.build()->img, "image");
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(OcrTest)
{
	pbuilder.set_ocr_path("ocr");
	BOOST_CHECK_EQUAL(pbuilder.build()->ocr, "ocr");
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(BoxTest)
{
	pbuilder.set_box(Box{1, 2, 3, 4});
	BOOST_CHECK_EQUAL(pbuilder.build()->box, Box(1, 2, 3, 4));
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_SUITE_END()

struct AppendLineFixture: PageBuilderFixture {
	LineBuilder lbuilder;
};

////////////////////////////////////////////////////////////////////////////////
BOOST_FIXTURE_TEST_SUITE(AppendLine, AppendLineFixture)

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(SingleAppendTest)
{
	auto l = lbuilder.build();
	pbuilder.append(*l);
	auto p = pbuilder.build();
	BOOST_CHECK_EQUAL(l->id(), 1);
	BOOST_CHECK_EQUAL(&l->page(), p.get());
	BOOST_CHECK_EQUAL(p->find(1), l);
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(MultipleAppendTest)
{
	std::vector<LineSptr> lines(5);
	for (auto& l: lines) {
		lbuilder.reset();
		l = lbuilder.build();
		pbuilder.append(*l);
	}

	auto p = pbuilder.build();
	int i = 1;
	for (const auto& l: lines) {
		BOOST_CHECK_EQUAL(l->id(), i);
		BOOST_CHECK_EQUAL(&l->page(), p.get());
		BOOST_CHECK_EQUAL(p->find(i), l);
		++i;
	}
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_SUITE_END()
