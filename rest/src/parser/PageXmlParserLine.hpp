#ifndef pcw_PageXmlParserLine_hpp__
#define pcw_PageXmlParserLine_hpp__

#include "ParserPage.hpp"
#include "Xml.hpp"

namespace pcw {
class PageXmlParserLine : public ParserLine
{
public:
  PageXmlParserLine(const Xml::Node& line);
  virtual void end_wagner_fischer() override;
  virtual void insert(size_t i, wchar_t c) override;
  virtual void erase(size_t i) override;
  virtual void set(size_t i, wchar_t c) override;
  virtual std::wstring wstring() const override;
  virtual std::string string() const override;
  virtual LinePtr line(int id) const override;
};
}

#endif // pcw_PageXmlParserLine_hpp__
