#ifndef profiler_RemoteProfiler_hpp__
#define profiler_RemoteProfiler_hpp__

#include "Profiler.hpp"

namespace profiler {
	class RemoteProfiler: public Profiler {
	public:
		RemoteProfiler(ConfigPtr config)
			: Profiler(std::move(config))
		{}
		virtual ~RemoteProfiler() noexcept override = default;

	protected:
		virtual pcw::Profile do_profile() override;
	};
}

#endif // profiler_RemoteProfiler_hpp__
