#ifndef pcw_LoginRoute_hpp__
#define pcw_LoginRoute_hpp__

#include "Route.hpp"

namespace pcw {
	class LoginRoute: public Route {
	public:
		virtual ~LoginRoute() noexcept override = default;
		virtual const char *route() const noexcept override {return route_;}
		virtual const char *name() const noexcept override {return name_;}
		virtual void Register(App&) override;
		crow::response operator()(const std::string& user, const std::string& pass) const;

	private:
		static const char* route_;
		static const char* name_;
	};
}

#endif // pcw_LoginRoute_hpp__

