#ifndef profiler_ProfilerRoute_hpp__
#define profiler_ProfilerRoute_hpp__

#include <memory>
#include <mutex>
#include <crow/logging.h>
#include "core/CrtpRoute.hpp"

namespace pcw {
	class Profile;
	template<class T> class Maybe;
}

namespace profiler {
	class Config;
	using ConfigPtr = std::shared_ptr<Config>;
	class Profiler;
	using ProfilerPtr = std::unique_ptr<Profiler>;

	class ProfilerRoute: public pcw::CrtpRoute<ProfilerRoute> {
	public:
		ProfilerRoute(ConfigPtr config);

		virtual void Register(App& app) override;
		virtual const char* route() const noexcept override {return route_;}
		virtual const char* name() const noexcept override {return name_;}

		pcw_crtp_route_def_impl__(int);
		Response impl(HttpGet, const Request& req, int bid) const;
		Response impl(HttpPost, const Request& req, int bid) const;

	private:
		using Mutex = std::mutex;
		using MutexPtr = std::shared_ptr<Mutex>;
		using Lock = std::lock_guard<Mutex>;
		using Jobs = std::unordered_map<int, std::future<pcw::Maybe<pcw::Profile>>>;
		using JobsPtr = std::shared_ptr<Jobs>;

		ProfilerPtr get_profiler() const;

		static const char *route_, *name_;
		MutexPtr mutex_;
		ConfigPtr config_;
		JobsPtr jobs_;
	};
}

#endif // profiler_ProfilerRoute_hpp__
