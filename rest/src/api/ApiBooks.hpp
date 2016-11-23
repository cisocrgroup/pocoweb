#ifndef pcw_ApiBooks_hpp__
#define pcw_ApiBooks_hpp__

#include "CrtpRoute.hpp"

namespace pcw {
	class Line;
	class Page;
	class BookView;
	using BookViewPtr = std::shared_ptr<BookView>;
	using PagePtr = std::shared_ptr<Page>;
	using LinePtr = std::shared_ptr<Line>;

	class ApiBooks: public CrtpRoute<ApiBooks> {
	public:
		virtual ~ApiBooks() noexcept override = default;
		virtual void Register(App& app) override;
		virtual const char* route() const noexcept override {return route_;}
		virtual const char* name() const noexcept override {return name_;}

		template<class T, class... Args>
		[[noreturn]] Response impl(T, const Request& req, Args&&... args) const {
			THROW(NotImplemented, "Not implemented: ", req.url,
					": ", typeid(T).name());
		}

		Response impl(HttpPost, const Request& req) const;
		Response impl(HttpGet, const Request& req) const;
		Response impl(HttpGet, const Request& req, int bid) const;
		Response impl(HttpPost, const Request& req, int bid) const;
		Response impl(HttpGet, const Request& req, int bid, int pid) const;
		Response impl(HttpGet, const Request& req, int bid, int pid, int lid) const;
		Response impl(HttpPut, const Request& req, int bid, int pid, int lid) const;

	private:
		BookViewPtr find(const Database& db, int bid) const;
		PagePtr find(const Database& db, int bid, int pid) const;
		LinePtr find(const Database& db, int bid, int pid, int lid) const;

		static const char* route_;
		static const char* name_;
	};
}

#endif // pcw_ApiBooks_hpp__
