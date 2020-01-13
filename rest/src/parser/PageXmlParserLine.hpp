#ifndef pcw_PageXmlParserLine_hpp__
#define pcw_PageXmlParserLine_hpp__

#include "ParserPage.hpp"
#include "Xml.hpp"
#include "pugixml.hpp"
#include <vector>

namespace pcw {
class PageXmlParserLine : public ParserLine {
public:
  PageXmlParserLine(const Xml::Node &line, bool first);
  virtual void end_wagner_fischer() override;
  virtual void insert(size_t i, wchar_t c) override;
  virtual void erase(size_t i) override;
  virtual void set(size_t i, wchar_t c) override;
  virtual std::wstring wstring() const override;
  virtual std::string string() const override;
  virtual LinePtr line(int id) const override;

private:
  void parse();
  void add_corrections_to_regions(const std::string &cor);
  void add_corrections_to_line(const std::string &cor);
  void add_corrections_to_words(const std::string &cor);

  struct glyph {
    Xml::Node glyph;
  };
  struct word {
    Xml::Node word;
    std::vector<glyph> glyphs;
  };
  Xml::Node node_;
  std::wstring string_;
  std::vector<word> words_;
  const bool first_;
};
} // namespace pcw

#endif // pcw_PageXmlParserLine_hpp__
