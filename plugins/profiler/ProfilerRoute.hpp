#ifndef profiler_ProfilerRoute_hpp__
#define profiler_ProfilerRoute_hpp__

#include <memory>
#include <mutex>
#include <crow/logging.h>
#include "core/CrtpRoute.hpp"

namespace profiler {
	class Config;
	using ConfigPtr = std::shared_ptr<Config>;

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
		static const char *route_, *name_;
		MutexPtr mutex_;
		ConfigPtr config_;
	};
}

#endif // profiler_ProfilerRoute_hpp__
