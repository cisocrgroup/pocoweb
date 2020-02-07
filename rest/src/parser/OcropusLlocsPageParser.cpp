#include "OcropusLlocsPageParser.hpp"
#include "OcropusLlocsParserLine.hpp"
#include "OcropusLlocsParserPage.hpp"
#include "ParserPage.hpp"
#include "core/Box.hpp"
#include "core/Line.hpp"
#include "core/Page.hpp"
#include "core/util.hpp"
#include "llocs.hpp"
#include "utils/Error.hpp"
#include <boost/filesystem/operations.hpp>
#include <boost/foreach.hpp>
#include <crow/logging.h>
#include <cstring>
#include <fstream>
#include <iostream>
#include <regex>

namespace fs = boost::filesystem;
using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
OcropusLlocsPageParser::OcropusLlocsPageParser(Path path)
    : dir_(path), id_(0), done_(false) {
  if (not fs::is_directory(dir_)) {
    THROW(Error, "(OcropusLlocsPageParser) not a directory: ", dir_);
  }
  CROW_LOG_DEBUG << "(OcropusLlocsPageParser) dir: " << dir_;
  id_ = guess_id(dir_);
}

////////////////////////////////////////////////////////////////////////////////
ParserPagePtr OcropusLlocsPageParser::parse() {
  assert(not done_);
  done_ = true; // one page per directory
  return parse_page();
}

////////////////////////////////////////////////////////////////////////////////
ParserPagePtr OcropusLlocsPageParser::parse_page() const {
  std::vector<PathPair> llocs;
  fs::directory_iterator b(dir_), e;
  BOOST_FOREACH (const auto &file, std::make_pair(b, e)) {
    const auto pair = get_path_pair(file);
    if (pair) {
      llocs.push_back(*pair);
    }
  }
  std::sort(begin(llocs), end(llocs),
            [](const auto &a, const auto &b) { return a.img < b.img; });
  auto page = std::make_shared<OcropusLlocsParserPage>(id_);
  for (int i = 0; i < static_cast<int>(llocs.size()); ++i) {
    page->lines().push_back(std::make_shared<OcropusLlocsParserLine>(
        i + 1, llocs[i].llocs, llocs[i].img));
  }
  page->ocr = dir_;
  page->file_type = FileType::Llocs;
  return page;
}

////////////////////////////////////////////////////////////////////////////////
boost::optional<OcropusLlocsPageParser::PathPair>
OcropusLlocsPageParser::get_path_pair(const Path &file) {
  // match on .png image files that have an associated .llocs file
  if (fs::is_regular_file(file) and file.extension() == ".png") {
    const auto llocs = get_llocs_from_png(file);
    if (fs::is_regular_file(llocs))
      return PathPair{llocs, file};
    CROW_LOG_WARNING << "(OcropusLlocsPageParser) cannot find llocs file for: "
                     << file << " (" << llocs << ")";
  }
  return {};
}
