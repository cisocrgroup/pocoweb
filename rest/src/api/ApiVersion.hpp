#ifndef pcw_ApiVersion_hpp__
#define pcw_ApiVersion_hpp__

#include "core/CrtpRoute.hpp"

namespace pcw {
	// this uses CrtpRoute just as reference
	class ApiVersion: public CrtpRoute<ApiVersion> {
	public:
		virtual ~ApiVersion() noexcept override = default;
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

#endif // pcw_ApiVersion_hpp__
