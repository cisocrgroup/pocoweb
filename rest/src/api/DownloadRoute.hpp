#ifndef pcw_DownloadRoute_hpp__
#define pcw_DownloadRoute_hpp__

#include "core/CrtpRoute.hpp"

namespace pcw {
class DownloadRoute : public CrtpRoute<DownloadRoute> {
       public:
	virtual ~DownloadRoute() noexcept override = default;
	virtual void Register(App& app) override;
	virtual const char* route() const noexcept override { return route_; }
	virtual const char* name() const noexcept override { return name_; }

	pcw_crtp_route_def_impl__(int);
	Response impl(HttpGet, const Request& req, int bid) const;

       private:
	Response download(const Request& req, int bid) const;

	static const char* route_;
	static const char* name_;
};
}

#endif  // pcw_DownloadRoute_hpp__
