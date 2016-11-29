#ifndef profiler_LocalProfiler_hpp__
#define profiler_LocalProfiler_hpp__

#include "Profiler.hpp"

namespace profiler {
	class LocalProfiler: public Profiler {
	public:
		LocalProfiler(BookPtr book): Profiler(std::move(book)) {}
		virtual ~LocalProfiler() noexcept override = default;

	protected:
		virtual pcw::Profile do_profile() override;
	};
}

#endif // profiler_LocalProfiler_hpp__
