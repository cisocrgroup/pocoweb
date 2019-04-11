#ifndef pcw_FinishRoute_hpp__
#define pcw_FinishRoute_hpp__

#include "core/CrtpRoute.hpp"

namespace crow {
namespace json {
class rvalue;
}
}

namespace pcw {
class Book;
class Project;
using ProjectPtr = std::shared_ptr<Project>;

class FinishRoute : public CrtpRoute<FinishRoute> {
       public:
	virtual ~FinishRoute() noexcept override = default;
	virtual void Register(App& app) override;
	virtual const char* route() const noexcept override { return route_; }
	virtual const char* name() const noexcept override { return name_; }

	pcw_crtp_route_def_impl__(int);
	Response impl(HttpPost, const Request& req, int bid) const;
  	Response impl(HttpGet, const Request& req, int bid) const;

       private:
	Response finish(const Request& req, int bid) const;

	static const char* route_;
	static const char* name_;
};
}

#endif  // pcw_FinishRoute_hpp__
