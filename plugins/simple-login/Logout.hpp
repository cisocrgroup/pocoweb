#ifndef pcw_Logout_hpp__
#define pcw_Logout_hpp__

#include <array>
#include <boost/optional.hpp>
#include "core/CrtpRoute.hpp"

namespace pcw {
class Logout : public CrtpRoute<Logout> {
       public:
	virtual ~Logout() noexcept override = default;
	virtual const char* route() const noexcept override { return route_; }
	virtual const char* name() const noexcept override { return name_; }
	virtual void Register(App&) override;

	pcw_crtp_route_def_impl__();
	Response impl(HttpGet, const Request& request) const;

       private:
	static const char* route_;
	static const char* name_;
};
}

#endif  // pcw_Logout_hpp__
