#ifndef pcw_CreateUser_hpp__
#define pcw_CreateUser_hpp__

#include "api/CrtpRoute.hpp"

namespace pcw {
	class CreateUser: public CrtpRoute<CreateUser> {
	public:
		virtual ~CreateUser() noexcept override = default;
		virtual const char *route() const noexcept override {return route_;}
		virtual const char *name() const noexcept override {return name_;}
		virtual void Register(App&) override;

		template<class T, class... Args>
		[[noreturn]] Response impl(T, const Request& req, Args&&... args) const {
			THROW(NotImplemented, "Not implemented: ", req.url,
					": ", typeid(T).name());
		}
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

