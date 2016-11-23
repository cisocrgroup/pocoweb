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

		template<class T, class... Args>
		Response impl(T t, const Request& req, Args&&... args) const {
			return default_impl(t, req, std::forward<Args>(args)...);
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

