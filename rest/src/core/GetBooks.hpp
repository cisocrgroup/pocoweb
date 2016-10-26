#ifndef pcw_GetBooks_hpp__
#define pcw_GetBooks_hpp__

#include "PostRoute.hpp"

namespace pcw {
	class GetBooks: public PostRoute {
	public:
		virtual ~GetBooks() noexcept override = default;
		virtual void Register(App& app) override;
		virtual const char* route() const noexcept override {return route_;}
		virtual const char* name() const noexcept override {return name_;}
		crow::response operator()(const crow::request& req, int prid) const;
		crow::response operator()(const crow::request& req, int prid, int pageid) const;
		crow::response operator()(const crow::request& req, int prid, int pageid, int lineid) const;

	private:
		static const char* route_;
		static const char* name_;
	};
}

#endif // pcw_GetBooks_hpp__
