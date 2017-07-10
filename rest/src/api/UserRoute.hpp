#ifndef pcw_UserRoute_hpp__
#define pcw_UserRoute_hpp__

#include "core/CrtpRoute.hpp"

namespace pcw {
	class QueryParser;
	class User;

	class UserRoute: public CrtpRoute<UserRoute> {
	public:
		virtual ~UserRoute() noexcept override = default;
		virtual void Register(App& app) override;
		virtual const char* route() const noexcept override {return route_;}
		virtual const char* name() const noexcept override {return name_;}

		pcw_crtp_route_def_impl__();
		pcw_crtp_route_def_impl__(int);
		Response impl(HttpGet, const Request& req) const;
		Response impl(HttpPost, const Request& req) const;
		Response impl(HttpDelete, const Request& req, int uid) const;
		[[noreturn]] Response impl(HttpGet, const Request& req, int uid) const;
		[[noreturn]] Response impl(HttpPost, const Request& req, int uid) const;

	private:
		static const char* route_;
		static const char* name_;
	};
}

#endif // pcw_UserRoute_hpp__
