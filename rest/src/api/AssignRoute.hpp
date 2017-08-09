#ifndef pcw_AssignRoute_hpp__
#define pcw_AssignRoute_hpp__

#include "core/CrtpRoute.hpp"

namespace pcw {
class AssignRoute : public CrtpRoute<AssignRoute> {
       public:
	virtual ~AssignRoute() noexcept override = default;
	virtual void Register(App& app) override;
	virtual const char* route() const noexcept override { return route_; }
	virtual const char* name() const noexcept override { return name_; }

	pcw_crtp_route_def_impl__(int);
	Response impl(HttpPost, const Request& req, int bid) const;

       private:
	Response assign(const Request& req, int bid) const;

	static const char* route_;
	static const char* name_;
};
}

#endif  // pcw_AssignRoute_hpp__
