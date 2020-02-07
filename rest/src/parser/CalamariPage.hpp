#ifndef pcw_CalamariPage_hpp__
#define pcw_CalamariPage_hpp__

#include "BasicParserPage.hpp"
#include "PageParser.hpp"
#include <boost/filesystem/path.hpp>
#include <boost/optional.hpp>
#include <memory>

namespace pcw {
class ParserPage;
using ParserPagePtr = std::shared_ptr<ParserPage>;
class Line;
using LinePtr = std::shared_ptr<Line>;
using Path = boost::filesystem::path;

class CalamariPageParser : public PageParser {
public:
  CalamariPageParser(Path dir);
  virtual ~CalamariPageParser() noexcept override = default;
  virtual bool has_next() const noexcept override { return false; }
  virtual ParserPagePtr parse() override;

private:
  struct PathPair {
    Path proto, img;
  };
  ParserPagePtr parse_page() const;

  static boost::optional<PathPair> get_path_pair(const Path &file);

  const Path dir_;
  int id_;
};

class CalamariParserPage : public BasicParserPage {
public:
  virtual ~CalamariParserPage() noexcept override = default;
  virtual void write(const Path &path) const override;
};
} // namespace pcw

#endif // pcw_CalamariPage_hpp__
