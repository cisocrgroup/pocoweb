#ifndef pcw_DeleteUser_hpp__
#define pcw_DeleteUser_hpp__

#include "Route.hpp"

namespace pcw {
	class DeleteUser: public Route {
	public:
		virtual ~DeleteUser() noexcept override = default;
		virtual const char *route() const noexcept override {return route_;}
		virtual const char *name() const noexcept override {return name_;}
		virtual void Register(App&) override;
		crow::response operator()(const std::string& user) const;

	private:
		static const char* route_;
		static const char* name_;
	};
}

#endif // pcw_DeleteUser_hpp__

