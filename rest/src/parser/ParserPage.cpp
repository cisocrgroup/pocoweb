#include "ParserPage.hpp"
#include "core/Page.hpp"
#include "core/WagnerFischer.hpp"
#include "core/util.hpp"
#include <crow/logging.h>
#include <fstream>
#include <iostream>
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
