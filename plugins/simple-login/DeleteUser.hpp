#ifndef pcw_DeleteUser_hpp__
#define pcw_DeleteUser_hpp__

#include "core/Route.hpp"

namespace pcw {
	class DeleteUser: public Route {
	public:
		virtual ~DeleteUser() noexcept override = default;
		virtual const char *route() const noexcept override {return route_;}
		virtual const char *name() const noexcept override {return name_;}
		virtual void Register(App&) override;

		Response operator()(const Request& req, const std::string& name) const;

	private:
		static const char* route_;
		static const char* name_;
	};
}

#endif // pcw_DeleteUser_hpp__

