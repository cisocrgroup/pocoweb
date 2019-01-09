#ifndef pcw_AltoXmlPageParser_hpp__
#define pcw_AltoXmlPageParser_hpp__

#include "PageParser.hpp"
#include "Xml.hpp"
#include <boost/filesystem/path.hpp>
#include <memory>

namespace pcw {
class Box;
class XmlParserPage;
using XmlParserPagePtr = std::shared_ptr<XmlParserPage>;

class AltoXmlPageParser : public PageParser
{
public:
  explicit AltoXmlPageParser(const Path& path)
    : path_(path)
    , xml_(path)
    , done_(false)
    , explicit_spaces_()
  {}
  explicit AltoXmlPageParser(Xml xml)
    : xml_(xml)
    , done_(false)
    , explicit_spaces_()
  {}
  virtual ~AltoXmlPageParser() noexcept override = default;
  virtual bool has_next() const noexcept override { return not done_; }
  virtual ParserPagePtr parse() override;

private:
  void add_line(const Xml::Node& linenode, XmlParserPage& page) const;
  void parse(const Xml::Node& pagenode, XmlParserPage& page) const;
  static Box get_box(const Xml::Node& node);
  static bool has_sp_tags(const XmlParserPage& page) noexcept;

  Path path_;
  Xml xml_;
  bool done_, explicit_spaces_;
};
}

#endif // pcw_AltoXmlPageParser_hpp__
