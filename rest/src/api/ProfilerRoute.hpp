#ifndef pcw_ProfilerRoute_hpp__
#define pcw_ProfilerRoute_hpp__

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
class Candidate;
class Token;

class ProfilerRoute : public CrtpRoute<ProfilerRoute> {
       public:
	ProfilerRoute();
	virtual ~ProfilerRoute() noexcept;

	virtual void Register(App& app) override;
	virtual const char* route() const noexcept override { return route_; }
	virtual const char* name() const noexcept override { return name_; }

	pcw_crtp_route_def_impl__(int);
	pcw_crtp_route_def_impl__();
	Response impl(HttpGet, const Request& req) const;
	Response impl(HttpGet, const Request& req, int bid) const;
	Response impl(HttpPost, const Request& req, int bid) const;

       private:
	struct Job {
		std::thread t;
		int id;
		std::atomic<bool> running;
	};
	using Lock = std::lock_guard<std::mutex>;
	ProfilerUptr get_profiler(ConstBookSptr book) const;
	Job* find_free_job(int id) const noexcept;
	static void profile(const ProfilerRoute* that,
			    ConstBookSptr book) noexcept;
	static void insert_profile(const ProfilerRoute* that,
				   const Profile& profile);

	std::shared_ptr<std::mutex> mutex_;
	std::shared_ptr<std::vector<Job>> jobs_;
	static const char *route_, *name_;
};
}

#endif  // pcw_ProfilerRoute_hpp__
