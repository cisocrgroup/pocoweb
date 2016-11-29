#ifndef profiler_Profiler_hpp__
#define profiler_Profiler_hpp__

#include <atomic>
#include <memory>

namespace pcw {
	class Profile;
}

namespace profiler {
	class Config;
	using ConfigPtr = std::shared_ptr<Config>;

	class Profiler {
	public:
		Profiler(ConfigPtr config)
			: config_(std::move(config))
			, done_(false)
		{}
		virtual ~Profiler() noexcept = default;
		bool done() const noexcept {return done_;}


	protected:
		virtual pcw::Profile do_profile() = 0;
		void set_done() noexcept {done_ = true;}
		const Config& config() const noexcept {return *config_;}

	private:
		ConfigPtr config_;
		std::atomic_bool done_;
	};
}

#endif // profiler_Profiler_hpp__
