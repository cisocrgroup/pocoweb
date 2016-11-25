#ifndef pcw_VersionRoute_hpp__
#define pcw_VersionRoute_hpp__

#include "core/CrtpRoute.hpp"

namespace pcw {
	// this uses CrtpRoute just as reference
	class VersionRoute: public CrtpRoute<VersionRoute> {
	public:
		virtual ~VersionRoute() noexcept override = default;
		virtual void Register(App& app) override;
		virtual const char* route() const noexcept override {return route_;}
		virtual const char* name() const noexcept override {return name_;}

		pcw_crtp_route_def_impl__();
		Response impl(HttpGet, const Request&) const noexcept;

	private:
		static const char* route_;
		static const char* name_;
	};
}

#endif // pcw_VersionRoute_hpp__
