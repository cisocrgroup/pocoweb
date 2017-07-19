#ifndef pcw_CorrectionRoute_hpp__
#define pcw_CorrectionRoute_hpp__

#include "core/CrtpRoute.hpp"

namespace pcw {
class Line;
class CorrectionRoute : public CrtpRoute<CorrectionRoute> {
       public:
	virtual ~CorrectionRoute() noexcept override = default;
	virtual void Register(App& app) override;
	virtual const char* route() const noexcept override { return route_; }
	virtual const char* name() const noexcept override { return name_; }

	pcw_crtp_route_def_impl__();
	Response impl(HttpPost, const Request& req) const;

       private:
	Response impl(MysqlConnection& conn, Line& line,
		      const std::string& c) const;
	Response impl(MysqlConnection& conn, Line& line, int tid,
		      const std::string& c) const;
	static void update_line(MysqlConnection& conn, const Line& line);
	static const char* route_;
	static const char* name_;
};
}

#endif  // pcw_CorrectionRoute_hpp__
