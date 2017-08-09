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
};
}

#endif  // pcw_RemoteProfiler_hpp__
