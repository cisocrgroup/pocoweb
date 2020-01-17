#ifndef pcw_PageXmlPageParser_hpp__
#define pcw_PageXmlPageParser_hpp__

#include "PageParser.hpp"
#include "Xml.hpp"
#include <boost/filesystem/path.hpp>
#include <memory>

namespace pcw {
class Box;
class XmlParserPage;
using XmlParserPagePtr = std::shared_ptr<XmlParserPage>;

class PageXmlPageParser : public PageParser {
public:
  explicit PageXmlPageParser(const Path &path)
      : path_(path), xml_(path), done_(false) {}
  explicit PageXmlPageParser(Xml xml) : xml_(xml), done_(false) {}
  virtual ~PageXmlPageParser() noexcept override = default;
  virtual bool has_next() const noexcept override { return not done_; }
  virtual ParserPagePtr parse() override;
  const Path &get_path() const noexcept { return path_; }

private:
  void add_line(const Xml::Node &linenode, XmlParserPage &page) const;
  void parse(const Xml::Node &pagenode, XmlParserPage &page) const;
  void parse_line(const Xml::Node &line_node, XmlParserPage &page) const;
  static Box get_page_box(const Xml::Node &node);

  Path path_;
  Xml xml_;
  bool done_;
};
} // namespace pcw
#endif // pcw_PageXmlPageParser_hpp__
