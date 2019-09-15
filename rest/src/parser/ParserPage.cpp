#include "ParserPage.hpp"
#include "core/Page.hpp"
#include "core/WagnerFischer.hpp"
#include "core/util.hpp"
#include <crow/logging.h>
#include <fstream>
#include <iostream>
#include <regex>
#include <utf8.h>

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
size_t ParserLine::correct(WagnerFischer &wf) {
  wf.set_ocr(wstring());
  const auto lev = wf();
  // CROW_LOG_DEBUG << "(ParserLine)  id: " << line(0)->id();
  // CROW_LOG_DEBUG << "(ParserLine) ocr: " << utf8(wf.ocr());
  // CROW_LOG_DEBUG << "(ParserLine) cor: " << utf8(wf.gt());
  wf.correct(*this);
  return lev;
}

////////////////////////////////////////////////////////////////////////////////
PagePtr ParserPage::page() const {
  auto page = std::make_shared<Page>(id, box);
  page->ocr = ocr;
  page->img = img;
  page->file_type = file_type;
  const auto n = size();
  for (auto i = 0U; i < n; ++i) {
    auto id = static_cast<int>(i + 1);
    page->push_back(get(i).line(id));
  }
  return page;
}

////////////////////////////////////////////////////////////////////////////////
int pcw::guess_id_from_string(const std::string &str) {
  // search for last number in string
  static const std::regex re(R"re((\d+)[^\d]*$)re");
  std::smatch m;
  if (not std::regex_search(str, m, re)) { // no number: cannot guess id
    return 0;
  }
  return std::stoi(m[1]);
}

////////////////////////////////////////////////////////////////////////////////
int pcw::guess_id_from_path(const Path &path) {
  return guess_id_from_string(path.filename().string());
}
