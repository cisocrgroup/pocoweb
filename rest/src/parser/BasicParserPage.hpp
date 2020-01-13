#ifndef pcw_BasicParserPage_hpp__
#define pcw_BasicParserPage_hpp__

#include "ParserPage.hpp"
#include <memory>
#include <vector>

namespace pcw {
class BasicParserPage : public ParserPage
{
public:
  BasicParserPage() = default;
  virtual ~BasicParserPage() noexcept override = default;
  virtual size_t size() const noexcept override { return lines_.size(); }
  virtual const ParserLine& get(size_t i) const override { return *lines_[i]; }
  virtual ParserLine& get(size_t i) override { return *lines_[i]; }
  const std::vector<ParserLinePtr>& lines() const noexcept { return lines_; }
  std::vector<ParserLinePtr>& lines() noexcept { return lines_; }

private:
  std::vector<ParserLinePtr> lines_;
};
}

#endif // pcw_BasicParserPage_hpp__
