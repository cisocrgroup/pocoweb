#ifndef pcw_GetBooks_hpp__
#define pcw_GetBooks_hpp__

#include "PostRoute.hpp"

namespace pcw {
	class Line;
	class Page;
	class Project;
	using ProjectPtr = std::shared_ptr<Project>;
	using PagePtr = std::shared_ptr<Page>;

	class GetBooks: public PostRoute {
	public:
		virtual ~GetBooks() noexcept override = default;
		virtual void Register(App& app) override;
		virtual const char* route() const noexcept override {return route_;}
		virtual const char* name() const noexcept override {return name_;}
		Response operator()(const Request& req) const;
		Response operator()(const Request& req, int bid) const;
		Response operator()(const Request& req, int bid, int pid) const;
		Response operator()(const Request& req, int bid, int pid, int lid) const;

	private:
		Response get(const Request& req, int bid) const;
		Response post(const Request& req, int bid) const;
		Response put(const Request& req, Database& db, Line& line) const;
		Response get(const Line& line) const;
		ProjectPtr find(const Database& db, int bid) const;
		PagePtr find(const Database& db, int bid, int pid) const;

		static const char* route_;
		static const char* name_;
	};
}

#endif // pcw_GetBooks_hpp__
