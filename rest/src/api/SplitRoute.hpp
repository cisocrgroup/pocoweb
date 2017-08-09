#ifndef pcw_SplitRoute_hpp__
#define pcw_SplitRoute_hpp__

#include "core/CrtpRoute.hpp"

namespace pcw {
class Book;
class Project;
using ProjectPtr = std::shared_ptr<Project>;

class SplitRoute : public CrtpRoute<SplitRoute> {
       public:
	virtual ~SplitRoute() noexcept override = default;
	virtual void Register(App& app) override;
	virtual const char* route() const noexcept override { return route_; }
	virtual const char* name() const noexcept override { return name_; }

	pcw_crtp_route_def_impl__(int);
	Response impl(HttpPost, const Request& req, int bid) const;

       private:
	Response split(const Request& req, int bid) const;
	void split_random(const Book& book,
			  std::vector<ProjectPtr>& projs) const;
	void split_sequencial(const Book& book,
			      std::vector<ProjectPtr>& projs) const;

	static const char* route_;
	static const char* name_;
};
}

#endif  // pcw_SplitRoute_hpp__
