#ifndef pcw_CreateUser_hpp__
#define pcw_CreateUser_hpp__

#include "Route.hpp"

namespace pcw {
	class CreateUser: public Route {
	public:
		virtual ~CreateUser() noexcept override = default;
		virtual const char *route() const noexcept override {return route_;}
		virtual const char *name() const noexcept override {return name_;}
		virtual void Register(App&) override;
		crow::response operator()(const std::string& user, const std::string& email) const;

	private:
		static const char* route_;
		static const char* name_;
	};
}

#endif // pcw_CreateUser_hpp__

