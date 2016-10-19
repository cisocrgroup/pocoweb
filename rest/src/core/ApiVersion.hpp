#ifndef pcw_ApiVersion_hpp__
#define pcw_ApiVersion_hpp__

#include "Route.hpp"

namespace pcw {
	class ApiVersion: public Route {
	public:
		virtual ~ApiVersion() noexcept override = default;
		virtual void Register(App& app) override;
		virtual const char* route() const noexcept override {return route_;}
		virtual const char* name() const noexcept override {return name_;}
		crow::response operator()() const noexcept;

	private:
		static const char* route_;
		static const char* name_;
	};
}

#endif // pcw_ApiVersion_hpp__
