#ifndef pcw_LoggedIn_hpp__
#define pcw_LoggedIn_hpp__

#include "core/Route.hpp"

namespace pcw {
	class LoggedIn: public Route {
	public:
		virtual ~LoggedIn() noexcept override = default;
		virtual const char *route() const noexcept override {return route_;}
		virtual const char *name() const noexcept override {return name_;}
		virtual void Register(App&) override;
		Response operator()(const Request& request) const;

	private:
		static const char* route_;
		static const char* name_;
	};
}

#endif // pcw_LoggedIn_hpp__

