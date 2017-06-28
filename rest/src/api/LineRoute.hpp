#ifndef pcw_LineRoute_hpp__
#define pcw_LineRoute_hpp__

#include "core/CrtpRoute.hpp"

namespace pcw {
	class QueryParser;
	class WagnerFischer;

	class LineRoute: public CrtpRoute<LineRoute> {
	public:
		virtual ~LineRoute() noexcept override = default;
		virtual void Register(App& app) override;
		virtual const char* route() const noexcept override {return route_;}
		virtual const char* name() const noexcept override {return name_;}

		pcw_crtp_route_def_impl__(int,int,int);
		Response impl(HttpGet, const Request& req, int bid,
				int pid, int lid) const;
		Response impl(HttpPost, const Request& req, int bid,
				int pid, int lid) const;

	private:
		Response correct(MysqlConnection& conn, Line& line, const QueryParser& data) const;
		void log(const WagnerFischer& wf) const;
		static void print_with_dotted_circles(const std::wstring& str, std::string& u8);

		static const char* route_;
		static const char* name_;
	};
}

#endif // pcw_LineRoute_hpp__
