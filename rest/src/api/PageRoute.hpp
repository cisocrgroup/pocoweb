#ifndef pcw_PageRoute_hpp__
#define pcw_PageRoute_hpp__

#include "core/CrtpRoute.hpp"

namespace pcw {
	class Line;
	class Page;
	class BookView;
	using BookViewPtr = std::shared_ptr<BookView>;
	using PagePtr = std::shared_ptr<Page>;
	using LinePtr = std::shared_ptr<Line>;

	class PageRoute: public CrtpRoute<PageRoute> {
	public:
		virtual ~PageRoute() noexcept override = default;
		virtual void Register(App& app) override;
		virtual const char* route() const noexcept override {return route_;}
		virtual const char* name() const noexcept override {return name_;}

		pcw_crtp_route_def_impl__(int,int);
		pcw_crtp_route_def_impl__(int,int,std::string,int);
		Response impl(HttpGet, const Request& req, int bid, int pid) const;
		Response impl(HttpGet, const Request& req, int bid, int pid,
				const std::string& dir, int val) const;

	private:
		Response next(const Page& page, int val) const;
		Response prev(const Page& page, int val) const;

		static const char* route_;
		static const char* name_;
	};
}

#endif // pcw_PageRoute_hpp__
