#ifndef pcw_CreateUser_hpp__
#define pcw_CreateUser_hpp__

#include "core/CrtpRoute.hpp"

namespace pcw {
	class CreateUser: public CrtpRoute<CreateUser> {
	public:
		virtual ~CreateUser() noexcept override = default;
		virtual const char *route() const noexcept override {return route_;}
		virtual const char *name() const noexcept override {return name_;}
		virtual void Register(App&) override;

		pcw_crtp_route_def_impl__(std::string,std::string);
		crow::response impl(
			HttpPost,
			const crow::request& request,
			const std::string& name,
			const std::string& pass
		) const;

	private:
		static const char* route_;
		static const char* name_;
	};
}

#endif // pcw_CreateUser_hpp__

