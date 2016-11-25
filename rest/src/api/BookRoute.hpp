#ifndef pcw_BookRoute_hpp__
#define pcw_BookRoute_hpp__

#include "core/CrtpRoute.hpp"

namespace pcw {
	class BookRoute: public CrtpRoute<BookRoute> {
	public:
		virtual ~BookRoute() noexcept override = default;
		virtual void Register(App& app) override;
		virtual const char* route() const noexcept override {return route_;}
		virtual const char* name() const noexcept override {return name_;}

		pcw_crtp_route_def_impl__();
		pcw_crtp_route_def_impl__(int);
		Response impl(HttpPost, const Request& req) const;
		Response impl(HttpGet, const Request& req) const;
		Response impl(HttpGet, const Request& req, int bid) const;
		Response impl(HttpPost, const Request& req, int bid) const;

	private:
		static const char* route_;
		static const char* name_;
	};
}

#endif // pcw_BookRoute_hpp__
