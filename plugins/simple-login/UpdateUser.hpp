#ifndef pcw_UpdateUser_hpp__
#define pcw_UpdateUser_hpp__

#include "Route.hpp"

namespace pcw {
	class UpdateUser: public Route {
	public:
		virtual ~UpdateUser() noexcept override = default;
		virtual const char *route() const noexcept override {return route_;}
		virtual const char *name() const noexcept override {return name_;}
		virtual void Register(App&) override;
		crow::response operator()(const std::string& what, const std::string& val) const;

	private:
		static const char* route_;
		static const char* name_;
	};
}

#endif // pcw_UpdateUser_hpp__

