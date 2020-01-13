#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE TestDbStructs

#include "core/WagnerFischer.hpp"
#include "core/util.hpp"
#include "database/DbStructs.hpp"
#include <boost/test/unit_test.hpp>
#include <chrono>
#include <iostream>
#include <random>

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
static DbLine mline(const std::wstring &str) {
  DbLine ret(1, 2, 3);
  int i = 0;
  for (const auto c : str) {
    ret.line.push_back(
        DbChar{.ocr = c, .cor = 0, .cut = i++ * 10 + 10, .conf = 0.5});
  }
  ret.box.set_left(0);
  ret.box.set_right(int(str.size()) * 10);
  ret.box.set_top(0);
  ret.box.set_bottom(100);
  return ret;
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(TestDbLineOCR) {
  auto line = mline(L"abc");
  BOOST_CHECK_EQUAL(line.slice().ocr(), "abc");
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(TestDbLineCor) {
  auto line = mline(L"abc");
  BOOST_CHECK_EQUAL(line.slice().cor(), "abc");
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(TestDbLineTokens) {
  std::vector<std::string> want{"one", "token", "two", "tokens"};
  auto line = mline(L"one token two tokens");
  int i = 0;
  line.each_token(
      [&](const auto &slice) { BOOST_CHECK_EQUAL(slice.ocr(), want[i++]); });
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(TestDbLineTokenBoxes) {
  std::vector<Box> want{Box{0, 0, 30, 100}, Box{40, 0, 70, 100}};
  auto line = mline(L"one two");
  int i = 0;
  line.each_token(
      [&](const auto &slice) { BOOST_CHECK_EQUAL(slice.box, want[i++]); });
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(TestDbLineTokenSlices) {
  auto line = mline(L"one two three");
  BOOST_CHECK_EQUAL(line.slice(0, line.tokenLength(0)).cor(), "one");
  BOOST_CHECK_EQUAL(line.slice(4, line.tokenLength(4)).cor(), "two");
  BOOST_CHECK_EQUAL(line.slice(8, line.tokenLength(8)).cor(), "three");
  BOOST_CHECK_EQUAL(line.slice(3, 5).cor(), " two ");
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(TestDbLineCorrectEmptyLine) {
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
BOOST_AUTO_TEST_CASE(TestDbLineCorrectEmptyLineASecondTime) {
  auto line = mline(L"");
  auto slice = line.slice();
  WagnerFischer wf;
  wf.set_ocr(slice.wocr());
  wf.set_gt(L"abc");
  BOOST_CHECK_EQUAL(wf(), 3);
  wf.correct(slice);
  BOOST_CHECK_EQUAL(slice.ocr(), "");
  BOOST_CHECK_EQUAL(slice.cor(), "abc");
  // correct the line a second time
  wf.set_ocr(slice.wocr());
  wf.set_gt("XYZ");
  BOOST_CHECK_EQUAL(wf(), 3);
  wf.correct(slice);
  BOOST_CHECK_EQUAL(slice.ocr(), "");
  BOOST_CHECK_EQUAL(slice.cor(), "XYZ");
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(TestDbLineCorrectEmptyLineBox) {
  // cuts are dispersed half the difference to the left and the right
  std::vector<int> cuts{15, 22, 26};
  auto line = mline(L"");
  line.box = Box{0, 0, 30, 0};
  auto slice = line.slice();
  WagnerFischer wf;
  wf.set_ocr(slice.wocr());
  wf.set_gt(L"abc");
  BOOST_CHECK_EQUAL(wf(), 3);
  wf.correct(slice);
  int i = 0;
  std::for_each(slice.begin, slice.end,
                [&](const auto &c) { BOOST_CHECK_EQUAL(cuts[i++], c.cut); });
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(TestDbLineCorrectDelete) {
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
BOOST_AUTO_TEST_CASE(TestDbLineCorrectDeleteASecondTime) {
  auto line = mline(L"abc");
  auto slice = line.slice();
  WagnerFischer wf;
  wf.set_ocr(slice.wocr());
  wf.set_gt(L"");
  BOOST_CHECK_EQUAL(wf(), 3);
  wf.correct(slice);
  BOOST_CHECK_EQUAL(line.slice().ocr(), "abc");
  BOOST_CHECK_EQUAL(line.slice().cor(), "");
  // overwrite correction a second time
  wf.set_ocr(slice.wocr());
  wf.set_gt("yz");
  BOOST_CHECK_EQUAL(wf(), 3);
  wf.correct(slice);
  BOOST_CHECK_EQUAL(line.slice().ocr(), "abc");
  BOOST_CHECK_EQUAL(line.slice().cor(), "yz");
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(TestDbLineCorrectAtStart) {
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
BOOST_AUTO_TEST_CASE(TestDbLineCorrectInTheMiddle) {
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
BOOST_AUTO_TEST_CASE(TestDbLineCorrectAthTheEnd) {
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

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(TestDbLineCorrectAtStartMultipleTimes) {
  auto line = mline(L"abc def ghi");
  auto slice = line.slice(0, 3);
  WagnerFischer wf;
  wf.set_ocr(slice.wocr());
  wf.set_gt(L"XYZXYZ");
  BOOST_CHECK_EQUAL(wf(), 6);
  wf.correct(slice);
  BOOST_CHECK_EQUAL(line.slice().ocr(), "abc def ghi");
  BOOST_CHECK_EQUAL(line.slice().cor(), "XYZXYZ def ghi");
  // correct a second time
  wf.set_ocr(slice.wocr());
  wf.set_gt(L"ABC");
  BOOST_CHECK_EQUAL(wf(), 3);
  wf.correct(slice);
  BOOST_CHECK_EQUAL(line.slice().ocr(), "abc def ghi");
  BOOST_CHECK_EQUAL(line.slice().cor(), "ABC def ghi");
  // correct a third time
  wf.set_ocr(slice.wocr());
  wf.set_gt(L"");
  BOOST_CHECK_EQUAL(wf(), 3);
  wf.correct(slice);
  BOOST_CHECK_EQUAL(line.slice().ocr(), "abc def ghi");
  BOOST_CHECK_EQUAL(line.slice().cor(), " def ghi");
  // correct a fourth time
  wf.set_ocr(slice.wocr());
  wf.set_gt(L"XXXX");
  BOOST_CHECK_EQUAL(wf(), 4);
  wf.correct(slice);
  BOOST_CHECK_EQUAL(line.slice().ocr(), "abc def ghi");
  BOOST_CHECK_EQUAL(line.slice().cor(), "XXXX def ghi");
  // correct a fifth time
  wf.set_ocr(slice.wocr());
  wf.set_gt(L"abc");
  BOOST_CHECK_EQUAL(wf(), 0);
  wf.correct(slice);
  BOOST_CHECK_EQUAL(line.slice().ocr(), "abc def ghi");
  BOOST_CHECK_EQUAL(line.slice().cor(), "abc def ghi");
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(TestDbLineCorrectInTheMiddleMultipleTimes) {
  auto line = mline(L"abc def ghi");
  auto slice = line.slice(4, 3);
  WagnerFischer wf;
  wf.set_ocr(slice.wocr());
  wf.set_gt(L"XYZXYZ");
  BOOST_CHECK_EQUAL(wf(), 6);
  wf.correct(slice);
  BOOST_CHECK_EQUAL(line.slice().ocr(), "abc def ghi");
  BOOST_CHECK_EQUAL(line.slice().cor(), "abc XYZXYZ ghi");
  // correct a second time
  wf.set_ocr(slice.wocr());
  wf.set_gt(L"DEF");
  BOOST_CHECK_EQUAL(wf(), 3);
  wf.correct(slice);
  BOOST_CHECK_EQUAL(line.slice().ocr(), "abc def ghi");
  BOOST_CHECK_EQUAL(line.slice().cor(), "abc DEF ghi");
  // correct a third time
  wf.set_ocr(slice.wocr());
  wf.set_gt(L"");
  BOOST_CHECK_EQUAL(wf(), 3);
  wf.correct(slice);
  BOOST_CHECK_EQUAL(line.slice().ocr(), "abc def ghi");
  BOOST_CHECK_EQUAL(line.slice().cor(), "abc  ghi");
  // correct a fourth time
  wf.set_ocr(slice.wocr());
  wf.set_gt(L"XXXX");
  BOOST_CHECK_EQUAL(wf(), 4);
  wf.correct(slice);
  BOOST_CHECK_EQUAL(line.slice().ocr(), "abc def ghi");
  BOOST_CHECK_EQUAL(line.slice().cor(), "abc XXXX ghi");
  // correct a fifth time
  wf.set_ocr(slice.wocr());
  wf.set_gt(L"def");
  BOOST_CHECK_EQUAL(wf(), 0);
  wf.correct(slice);
  BOOST_CHECK_EQUAL(line.slice().ocr(), "abc def ghi");
  BOOST_CHECK_EQUAL(line.slice().cor(), "abc def ghi");
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(TestDbLineCorrectAthTheEndMultipleTimes) {
  auto line = mline(L"abc def ghi");
  auto slice = line.slice(8, 3);
  WagnerFischer wf;
  wf.set_ocr(slice.wocr());
  wf.set_gt(L"XYZXYZ");
  BOOST_CHECK_EQUAL(wf(), 6);
  wf.correct(slice);
  BOOST_CHECK_EQUAL(line.slice().ocr(), "abc def ghi");
  BOOST_CHECK_EQUAL(line.slice().cor(), "abc def XYZXYZ");
  // correct a second time
  wf.set_ocr(slice.wocr());
  wf.set_gt(L"GHI");
  BOOST_CHECK_EQUAL(wf(), 3);
  wf.correct(slice);
  BOOST_CHECK_EQUAL(line.slice().ocr(), "abc def ghi");
  BOOST_CHECK_EQUAL(line.slice().cor(), "abc def GHI");
  // correct a third time
  wf.set_ocr(slice.wocr());
  wf.set_gt(L"");
  BOOST_CHECK_EQUAL(wf(), 3);
  wf.correct(slice);
  BOOST_CHECK_EQUAL(line.slice().ocr(), "abc def ghi");
  BOOST_CHECK_EQUAL(line.slice().cor(), "abc def ");
  // correct a fourth time
  wf.set_ocr(slice.wocr());
  wf.set_gt(L"XXXX");
  BOOST_CHECK_EQUAL(wf(), 4);
  wf.correct(slice);
  BOOST_CHECK_EQUAL(line.slice().ocr(), "abc def ghi");
  BOOST_CHECK_EQUAL(line.slice().cor(), "abc def XXXX");
  // correct a fifth time
  wf.set_ocr(slice.wocr());
  wf.set_gt(L"ghi");
  BOOST_CHECK_EQUAL(wf(), 0);
  wf.correct(slice);
  BOOST_CHECK_EQUAL(line.slice().ocr(), "abc def ghi");
  BOOST_CHECK_EQUAL(line.slice().cor(), "abc def ghi");
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(TestDbLineCorrectWithNoCorrection) {
  auto line = mline(L"Dz ſolcher dienſt Gut vñ  Bluth gelde.");
  auto slice = line.slice(11, 6);
  WagnerFischer wf;
  wf.set_ocr(slice.wocr());
  wf.set_gt(L"dienſt");
  BOOST_CHECK_EQUAL(wf(), 0);
  wf.correct(slice);
  BOOST_CHECK_EQUAL(line.slice().ocr(),
                    "Dz ſolcher dienſt Gut vñ  Bluth gelde.");
  BOOST_CHECK_EQUAL(line.slice().cor(),
                    "Dz ſolcher dienſt Gut vñ  Bluth gelde.");
}

////////////////////////////////////////////////////////////////////////////////
static std::wstring randomString(int seed) {
  static const std::vector<wchar_t> chars{
      L'a', L'b', L'c', L'ä', L'ö', L'ü', L'ß',
      L'.', L'!', L'/', L'0', L'1', L'2', L'3',
  };
  static std::mt19937 gen = std::mt19937(seed);
  std::uniform_int_distribution<size_t> rlen(1, 100); // lenght of string
  std::uniform_int_distribution<size_t> rchar(0, chars.size()); // char indices
  std::wstring ret(rlen(gen), 0);
  for (auto &c : ret) {
    c = chars[rchar(gen)];
  }
  return ret;
}

////////////////////////////////////////////////////////////////////////////////
static DbSlice randomSlice(DbLine &line, int seed) {
  static std::mt19937 gen = std::mt19937(seed);
  std::uniform_int_distribution<size_t> rindex(0, line.slice().wocr().size());
  auto start = rindex(gen);
  size_t end = rindex(gen);
  if (start > end) {
    std::swap(start, end);
  }
  assert(start <= end);
  return line.slice(int(start), int(end - start));
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(TestDbLineCorrectFuzzy) {
  const auto seed = std::chrono::duration_cast<std::chrono::milliseconds>(
                        std::chrono::time_point_cast<std::chrono::milliseconds>(
                            std::chrono::high_resolution_clock::now())
                            .time_since_epoch())
                        .count();
  std::cout << "seed: " << seed << "\n";
  const auto ocr = randomString(seed);
  const int N = 100;
  WagnerFischer wf;
  auto line = mline(ocr);
  for (int i = 0; i < N; i++) {
    auto slice = randomSlice(line, seed);
    auto cor = randomString(seed);
    wf.set_ocr(slice.wocr());
    wf.set_gt(cor);
    wf();
    wf.correct(slice);
    BOOST_CHECK_EQUAL(line.slice().ocr(), utf8(ocr));
    BOOST_CHECK_EQUAL(slice.cor(), utf8(cor));
  }
}
