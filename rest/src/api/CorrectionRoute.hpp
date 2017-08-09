#ifndef pcw_CorrectionRoute_hpp__
#define pcw_CorrectionRoute_hpp__

#include <boost/optional.hpp>
#include "core/CrtpRoute.hpp"

namespace pcw {
class Line;
struct Token;
class CorrectionRoute : public CrtpRoute<CorrectionRoute> {
       public:
	virtual ~CorrectionRoute() noexcept override = default;
	virtual void Register(App& app) override;
	virtual const char* route() const noexcept override { return route_; }
	virtual const char* name() const noexcept override { return name_; }

	pcw_crtp_route_def_impl__(int, int, int);
	pcw_crtp_route_def_impl__(int, int, int, int);
	Response impl(HttpPost, const Request& req, int pid, int p,
		      int lid) const;
	Response impl(HttpPost, const Request& req, int pid, int p, int lid,
		      int tid) const;

       private:
	Response correct(MysqlConnection& conn, Line& line,
			 const std::string& c) const;
	Response correct(MysqlConnection& conn, Line& line, int tid,
			 const std::string& c) const;
	static boost::optional<Token> find_token(const Line& line, int tid);
	static void update_line(MysqlConnection& conn, const Line& line);
	static const char* route_;
	static const char* name_;
};
}

#endif  // pcw_CorrectionRoute_hpp__
