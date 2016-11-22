#ifndef pcw_ApiBooks_hpp__
#define pcw_ApiBooks_hpp__

#include "core/Route.hpp"

namespace pcw {
	class Line;
	class Page;
	class BookView;
	using BookViewPtr = std::shared_ptr<BookView>;
	using PagePtr = std::shared_ptr<Page>;

	class ApiBooks: public Route {
	public:
		virtual ~ApiBooks() noexcept override = default;
		virtual void Register(App& app) override;
		virtual const char* route() const noexcept override {return route_;}
		virtual const char* name() const noexcept override {return name_;}
		Response operator()(const Request& req) const;
		Response operator()(const Request& req, int bid) const;
		Response operator()(const Request& req, int bid, int pid) const;
		Response operator()(const Request& req, int bid, int pid, int lid) const;

	private:
		Response post(const Request& req) const;
		Response get(const Request& req) const;
		Response get(const Request& req, int bid) const;
		Response post(const Request& req, int bid) const;
		Response put(const Request& req, Database& db, Line& line) const;
		Response get(const Line& line) const;
		BookViewPtr find(const Database& db, int bid) const;
		PagePtr find(const Database& db, int bid, int pid) const;

		static const char* route_;
		static const char* name_;
	};
}

#endif // pcw_ApiBooks_hpp__
