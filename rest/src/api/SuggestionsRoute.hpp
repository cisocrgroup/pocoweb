#ifndef pcw_SuggestionsRoute_hpp__
#define pcw_SuggestionsRoute_hpp__

#include <crow/logging.h>
#include <future>
#include <memory>
#include <mutex>
#include <set>
#include "core/CrtpRoute.hpp"

namespace pcw {
class SuggestionsRoute : public CrtpRoute<SuggestionsRoute> {
       public:
	SuggestionsRoute() = default;
	virtual ~SuggestionsRoute() noexcept = default;

	virtual void Register(App& app) override;
	virtual const char* route() const noexcept override { return route_; }
	virtual const char* name() const noexcept override { return name_; }

	pcw_crtp_route_def_impl__(int);
	Response impl(HttpGet, const Request& req, int bid) const;
	pcw_crtp_route_def_impl__(int, int, int, int);
	Response impl(HttpGet, const Request& req, int pid, int p, int lid,
		      int tid) const;

       private:
	Response suggestions(const Request& req, int pid,
			     const boost::optional<std::string>& q,
			     bool p) const;
	static const char *route_, *name_;
};
}

#endif  // pcw_ProfilerRoute_hpp__
