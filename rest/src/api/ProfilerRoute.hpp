#ifndef profiler_ProfilerRoute_hpp__
#define profiler_ProfilerRoute_hpp__

#include <crow/logging.h>
#include <future>
#include <memory>
#include <mutex>
#include <set>
#include "core/CrtpRoute.hpp"

namespace pcw {
class Book;
using ConstBookSptr = std::shared_ptr<const Book>;
class Profile;
template <class T>
class Maybe;
using ConstBookSptr = ConstBookSptr;
class Profiler;
using ProfilerUptr = std::unique_ptr<Profiler>;
class Profile;

class ProfilerRoute : public CrtpRoute<ProfilerRoute> {
       public:
	ProfilerRoute();
	virtual ~ProfilerRoute() noexcept;

	virtual void Register(App& app) override;
	virtual const char* route() const noexcept override { return route_; }
	virtual const char* name() const noexcept override { return name_; }

	pcw_crtp_route_def_impl__(int);
	Response impl(HttpGet, const Request& req, int bid) const;
	Response impl(HttpPost, const Request& req, int bid) const;

       private:
	using Lock = std::lock_guard<std::mutex>;
	ProfilerUptr get_profiler(ConstBookSptr book) const;
	static void profile(const ProfilerRoute* that,
			    ProjectSessionObject obj);
	static void insert_profile(const Profile& profile,
				   ProjectSessionObject& obj,
				   double min_weight);

	std::shared_ptr<std::mutex> mutex_;
	std::shared_ptr<std::set<int>> jobs_;
	static const char *route_, *name_;
};
}

#endif  // profiler_ProfilerRoute_hpp__
