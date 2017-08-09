#ifndef pcw_LineRoute_hpp__
#define pcw_LineRoute_hpp__

#include "core/CrtpRoute.hpp"

namespace pcw {
class LineRoute : public CrtpRoute<LineRoute> {
       public:
	virtual ~LineRoute() noexcept override = default;
	virtual void Register(App& app) override;
	virtual const char* route() const noexcept override { return route_; }
	virtual const char* name() const noexcept override { return name_; }

	pcw_crtp_route_def_impl__(int, int, int);
	Response impl(HttpGet, const Request& req, int bid, int pid,
		      int lid) const;

       private:
	static const char* route_;
	static const char* name_;
};
}

#endif  // pcw_LineRoute_hpp__
