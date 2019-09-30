#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE TestDbStructs

#include "core/WagnerFischer.hpp"
#include "database/DbStructs.hpp"
#include <boost/test/unit_test.hpp>
#include <iostream>

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
static DbLine
mline(const std::wstring& str)
{
  DbLine ret(1, 2, 3);
  int i = 0;
  for (const auto c : str) {
    ret.line.push_back(
      DbChar{ .ocr = c, .cor = 0, .cut = i++ * 10 + 10, .conf = 0.5 });
  }
  ret.box.set_left(0);
  ret.box.set_right(int(str.size()) * 10);
  ret.box.set_top(0);
  ret.box.set_bottom(100);
  return ret;
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(TestDbLineOCR)
{
  auto line = mline(L"abc");
  BOOST_CHECK_EQUAL(line.slice().ocr(), "abc");
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(TestDbLineCor)
{
  auto line = mline(L"abc");
  BOOST_CHECK_EQUAL(line.slice().cor(), "abc");
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(TestDbLineTokens)
{
  std::vector<std::string> want{ "one", "token", "two", "tokens" };
  auto line = mline(L"one token two tokens");
  int i = 0;
  line.each_token(
    [&](const auto& slice) { BOOST_CHECK_EQUAL(slice.ocr(), want[i++]); });
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(TestDbLineTokenBoxes)
{
  std::vector<Box> want{ Box{ 0, 0, 30, 100 }, Box{ 40, 0, 70, 100 } };
  auto line = mline(L"one two");
  int i = 0;
  line.each_token(
    [&](const auto& slice) { BOOST_CHECK_EQUAL(slice.box, want[i++]); });
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(TestDbLineTokenSlices)
{
  auto line = mline(L"one two three");
  BOOST_CHECK_EQUAL(line.slice(0, line.tokenLength(0)).cor(), "one");
  BOOST_CHECK_EQUAL(line.slice(4, line.tokenLength(4)).cor(), "two");
  BOOST_CHECK_EQUAL(line.slice(8, line.tokenLength(8)).cor(), "three");
  BOOST_CHECK_EQUAL(line.slice(3, 5).cor(), " two ");
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(TestDbLineCorrectEmptyLine)
{
  auto line = mline(L"");
  auto slice = line.slice();
  WagnerFischer wf;
  wf.set_ocr(slice.wocr());
  wf.set_gt(L"abc");
  BOOST_CHECK_EQUAL(wf(), 3);
  wf.correct(slice);
  BOOST_CHECK_EQUAL(slice.ocr(), "");
  BOOST_CHECK_EQUAL(slice.cor(), "abc");
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(TestDbLineCorrectEmptyLineASecondTime)
{
  auto line = mline(L"");
  auto slice = line.slice();
  WagnerFischer wf;
  wf.set_ocr(slice.wocr());
  wf.set_gt(L"abc");
  BOOST_CHECK_EQUAL(wf(), 3);
  wf.correct(slice);
  BOOST_CHECK_EQUAL(slice.ocr(), "");
  BOOST_CHECK_EQUAL(slice.cor(), "abc");
  wf.set_ocr(slice.wocr());
  wf.set_gt("XYZ");
  BOOST_CHECK_EQUAL(wf(), 3);
  wf.correct(slice);
  BOOST_CHECK_EQUAL(slice.ocr(), "");
  BOOST_CHECK_EQUAL(slice.cor(), "XYZ");
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(TestDbLineCorrectEmptyLineBox)
{
  // cuts are dispersed half the difference to the left and the right
  std::vector<int> cuts{ 15, 22, 26 };
  auto line = mline(L"");
  line.box = Box{ 0, 0, 30, 0 };
  auto slice = line.slice();
  WagnerFischer wf;
  wf.set_ocr(slice.wocr());
  wf.set_gt(L"abc");
  BOOST_CHECK_EQUAL(wf(), 3);
  wf.correct(slice);
  int i = 0;
  std::for_each(slice.begin, slice.end, [&](const auto& c) {
    BOOST_CHECK_EQUAL(cuts[i++], c.cut);
  });
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(TestDbLineCorrectDelete)
{
  auto line = mline(L"abc");
  auto slice = line.slice();
  WagnerFischer wf;
  wf.set_ocr(slice.wocr());
  wf.set_gt(L"");
  BOOST_CHECK_EQUAL(wf(), 3);
  wf.correct(slice);
  BOOST_CHECK_EQUAL(line.slice().ocr(), "abc");
  BOOST_CHECK_EQUAL(line.slice().cor(), "");
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(TestDbLineCorrectAtStart)
{
  auto line = mline(L"abc def ghi");
  auto slice = line.slice(0, 3);
  WagnerFischer wf;
  wf.set_ocr(slice.wocr());
  wf.set_gt(L"XYZXYZ");
  BOOST_CHECK_EQUAL(wf(), 6);
  wf.correct(slice);
  BOOST_CHECK_EQUAL(line.slice().ocr(), "abc def ghi");
  BOOST_CHECK_EQUAL(line.slice().cor(), "XYZXYZ def ghi");
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(TestDbLineCorrectInTheMiddle)
{
  auto line = mline(L"abc def ghi");
  auto slice = line.slice(4, 3);
  WagnerFischer wf;
  wf.set_ocr(slice.wocr());
  wf.set_gt(L"XYZXYZ");
  BOOST_CHECK_EQUAL(wf(), 6);
  wf.correct(slice);
  BOOST_CHECK_EQUAL(line.slice().ocr(), "abc def ghi");
  BOOST_CHECK_EQUAL(line.slice().cor(), "abc XYZXYZ ghi");
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(TestDbLineCorrectAthTheEnd)
{
  auto line = mline(L"abc def ghi");
  auto slice = line.slice(8, 3);
  WagnerFischer wf;
  wf.set_ocr(slice.wocr());
  wf.set_gt(L"XYZXYZ");
  BOOST_CHECK_EQUAL(wf(), 6);
  wf.correct(slice);
  BOOST_CHECK_EQUAL(line.slice().ocr(), "abc def ghi");
  BOOST_CHECK_EQUAL(line.slice().cor(), "abc def XYZXYZ");
}
