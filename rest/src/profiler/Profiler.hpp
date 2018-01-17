#ifndef pcw_Profiler_hpp__
#define pcw_Profiler_hpp__

#include <atomic>
#include <memory>
#include <vector>

namespace pcw {
class Book;
using ConstBookSptr = std::shared_ptr<const Book>;
class Profile;
template <class T> class Maybe;
using ConstBookSptr = ConstBookSptr;

class Profiler {
public:
  Profiler(ConstBookSptr book) : book_(std::move(book)) {}
  virtual ~Profiler() noexcept = default;
  pcw::Maybe<Profile> profile() noexcept;
  pcw::Maybe<std::vector<std::string>> languages() noexcept;
  const Book &book() const noexcept { return *book_; }

protected:
  virtual Profile do_profile() = 0;
  virtual std::vector<std::string> do_languages() = 0;

private:
  ConstBookSptr book_;
};
}

#endif // pcw_Profiler_hpp__
