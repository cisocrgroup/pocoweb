#ifndef profiler_LocalProfiler_hpp__
#define profiler_LocalProfiler_hpp__

#include "Profiler.hpp"

namespace profiler {
	class LocalProfiler: public Profiler {
	public:
		virtual ~LocalProfiler() noexcept override = default;

	protected:
		virtual pcw::Profile do_profile() override;
	};
}

#endif // profiler_LocalProfiler_hpp__
