#ifndef pcw_LineRoute_hpp__
#define pcw_LineRoute_hpp__

#include "core/CrtpRoute.hpp"

namespace crow {
namespace json {
class rvalue;
}
}

namespace pcw {
class WagnerFischer;
struct Token;

class LineRoute : public CrtpRoute<LineRoute> {
       public:
	virtual ~LineRoute() noexcept override = default;
	virtual void Register(App& app) override;
	virtual const char* route() const noexcept override { return route_; }
	virtual const char* name() const noexcept override { return name_; }

	pcw_crtp_route_def_impl__(int, int, int);
	pcw_crtp_route_def_impl__(int, int, int, const std::string&);
	Response impl(HttpGet, const Request& req, int bid, int pid,
		      int lid) const;
	Response impl(HttpPost, const Request& req, int bid, int pid, int lid,
		      const std::string& str) const;

       private:
	Response create_split_images(const Session& session, const Line& line,
				     int tokenid) const;

	static const char* route_;
	static const char* name_;
};
}

#endif  // pcw_LineRoute_hpp__
