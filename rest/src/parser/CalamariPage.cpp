#include "CalamariPage.hpp"
#include "CalamariParserLine.hpp"
#include "utils/Error.hpp"
#include <boost/filesystem/operations.hpp>
#include <boost/foreach.hpp>
#include <fstream>

namespace fs = boost::filesystem;
using namespace pcw;

class CalamariParserPage : public BasicParserPage {
public:
  virtual ~CalamariParserPage() noexcept override = default;
  virtual void write(const Path &path) const override;
};

////////////////////////////////////////////////////////////////////////////////
CalamariPageParser::CalamariPageParser(Path dir)
    : dir_(dir), id_(std::stoi(dir.filename().string())) {
  if (not fs::is_directory(dir_)) {
    THROW(Error, "(CalamariPageParser) not a directory: ", dir_);
  }
}

////////////////////////////////////////////////////////////////////////////////
ParserPagePtr CalamariPageParser::parse() {
  std::vector<PathPair> preds;
  fs::directory_iterator b(dir_), e;
  BOOST_FOREACH (const auto &file, std::make_pair(b, e)) {
    const auto pair = get_path_pair(file);
    if (pair) {
      preds.push_back(*pair);
    }
  }
  std::sort(begin(preds), end(preds),
            [](const auto &a, const auto &b) { return a.proto < b.proto; });
  const auto parser_page = std::make_shared<CalamariParserPage>();
  for (int i = 0; i < int(preds.size()); i++) {
    parser_page->lines().push_back(
        std::make_shared<CalamariParserLine>(preds[i].proto, preds[i].img));
  }
  return parser_page;
}

////////////////////////////////////////////////////////////////////////////////
boost::optional<CalamariPageParser::PathPair>
CalamariPageParser::get_path_pair(const Path &file) {
  if (not fs::is_regular_file(file) or file.extension() != ".png") {
    return {};
  }
  for (auto i = file; file.has_extension();) {
    if (fs::is_regular_file(i.replace_extension(".pred"))) {
      return PathPair{.img = file, .proto = i.replace_extension(".pred")};
    }
    i = i.replace_extension("");
  }
  return {};
}

////////////////////////////////////////////////////////////////////////////////
void CalamariParserPage::write(const Path &dir) const {
  for (const auto& line: lines()) {
	const auto calamari = std::dynamic_pointer_cast<CalamariParserLine>(line);
	calamari->write(dir);
  }
}
