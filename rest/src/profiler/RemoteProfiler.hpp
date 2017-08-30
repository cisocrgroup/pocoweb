#ifndef pcw_RemoteProfiler_hpp__
#define pcw_RemoteProfiler_hpp__

#include "Profiler.hpp"

namespace pcw {
class RemoteProfiler : public Profiler {
       public:
	RemoteProfiler(ConstBookSptr book) : Profiler(std::move(book)) {}
	virtual ~RemoteProfiler() noexcept override = default;

       protected:
	virtual Profile do_profile() override;
	virtual std::vector<std::string> do_languages() override;
};
}

#endif  // pcw_RemoteProfiler_hpp__
