#ifndef pcw_CorrectionRoute_hpp__
#define pcw_CorrectionRoute_hpp__

#include "core/CrtpRoute.hpp"

namespace pcw {
class CorrectionRoute : public CrtpRoute<CorrectionRoute> {
       public:
	virtual ~CorrectionRoute() noexcept override = default;
	virtual void Register(App& app) override;
	virtual const char* route() const noexcept override { return route_; }
	virtual const char* name() const noexcept override { return name_; }

	pcw_crtp_route_def_impl__();
	Response impl(HttpPost, const Request& req) const;

       private:
	static const char* route_;
	static const char* name_;
};
}

#endif  // pcw_CorrectionRoute_hpp__
