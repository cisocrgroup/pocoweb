#ifndef pcw_CalamariParserLine_hpp__
#define pcw_CalamariParserLine_hpp__

#include "ParserPage.hpp"
#include "calamari.pb.h"

namespace pcw {
class CalamariParserLine : public ParserLine {
public:
  CalamariParserLine(Path proto, Path img);

  virtual ~CalamariParserLine() noexcept override = default;
  virtual void insert(size_t i, wchar_t c) override;
  virtual void erase(size_t i) override;
  virtual void set(size_t i, wchar_t c) override;
  virtual std::wstring wstring() const override;
  virtual std::string string() const override;
  virtual LinePtr line(int id) const override;

private:
  const Path proto_, img_;
  Prediction prediction_;
};
} // namespace pcw

#endif // pcw_CalamariParserLine_hpp__
