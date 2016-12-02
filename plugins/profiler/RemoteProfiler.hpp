#ifndef profiler_RemoteProfiler_hpp__
#define profiler_RemoteProfiler_hpp__

#include "Profiler.hpp"

namespace profiler {
	class RemoteProfiler: public Profiler {
	public:
		RemoteProfiler(ConstBookSptr book): Profiler(std::move(book)) {}
		virtual ~RemoteProfiler() noexcept override = default;

	protected:
		virtual pcw::Profile do_profile() override;
	};
}

#endif // profiler_RemoteProfiler_hpp__
