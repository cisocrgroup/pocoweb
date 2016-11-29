#ifndef profiler_Profiler_hpp__
#define profiler_Profiler_hpp__

#include <atomic>
#include <memory>

namespace pcw {
	class Profile;
	template<class T> class Maybe;
}

namespace profiler {
	class Config;
	using ConfigPtr = std::shared_ptr<Config>;

	class Profiler {
	public:
		virtual ~Profiler() noexcept = default;
		pcw::Maybe<pcw::Profile> profile() noexcept;

	protected:
		virtual pcw::Profile do_profile() = 0;
	};
}

#endif // profiler_Profiler_hpp__
