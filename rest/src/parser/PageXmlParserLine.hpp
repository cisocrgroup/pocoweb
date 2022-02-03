#ifndef pcw_PageXmlParserLine_hpp__
#define pcw_PageXmlParserLine_hpp__

#include "ParserPage.hpp"
#include "Xml.hpp"
#include "pugixml.hpp"
#include <vector>

namespace pcw {
class PageXmlParserLine : public ParserLine {
public:
  PageXmlParserLine(const Xml::Node &line);
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
  std::string mkname(const char* basename) const;
  bool first_line_in_region() const;
  bool checkWordBoxes() const;
  struct word;
  bool checkGlyphBoxes(const word& w) const;
  std::wstring get_text_equiv_unicode(const Xml::Node &node, int index) const;

  struct glyph {
    Xml::Node glyph;
	const Xml::Node& node() const {
	  return glyph;
	}
  };
  struct word {
    Xml::Node word;
    std::vector<glyph> glyphs;
	const Xml::Node& node() const {
	  return word;
	}
  };
  Xml::Node node_;
  std::wstring string_;
  std::vector<word> words_;
};
} // namespace pcw

#endif // pcw_PageXmlParserLine_hpp__
