#ifndef pcw_BookRoute_hpp__
#define pcw_BookRoute_hpp__

#include "core/CrtpRoute.hpp"

namespace pcw {
	class QueryParser;
	class Book;

	class BookRoute: public CrtpRoute<BookRoute> {
	public:
		virtual ~BookRoute() noexcept override = default;
		virtual void Register(App& app) override;
		virtual const char* route() const noexcept override {return route_;}
		virtual const char* name() const noexcept override {return name_;}

		pcw_crtp_route_def_impl__();
		pcw_crtp_route_def_impl__(int);
		pcw_crtp_route_def_impl__(std::string);
		Response impl(HttpPost, const Request& req) const;
		Response impl(HttpPost, const Request& req, const std::string& file) const;
		Response impl(HttpGet, const Request& req) const;
		Response impl(HttpGet, const Request& req, int bid) const;
		Response impl(HttpPost, const Request& req, int bid) const;

	private:
		Response set(const Request& req, int bid, const QueryParser& data) const;
		[[noreturn]] Response package(const Request& req, int bid, const QueryParser& data) const;
		void update_book_data(Book& book, const User& user, const QueryParser& data) const;

		static const char* route_;
		static const char* name_;
	};
}

#endif // pcw_BookRoute_hpp__
