#ifndef pcw_Login_hpp__
#define pcw_Login_hpp__

#include "core/Route.hpp"

namespace pcw {
	class Login: public Route {
	public:
		virtual ~Login() noexcept override = default;
		virtual const char *route() const noexcept override {return route_;}
		virtual const char *name() const noexcept override {return name_;}
		virtual void Register(App&) override;
		crow::response operator()(const std::string& name, const std::string& pass) const;

	private:
		static const char* route_;
		static const char* name_;
	};
}

#endif // pcw_Login_hpp__

