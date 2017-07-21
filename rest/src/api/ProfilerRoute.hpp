#ifndef profiler_ProfilerRoute_hpp__
#define profiler_ProfilerRoute_hpp__

#include <crow/logging.h>
#include <future>
#include <memory>
#include <mutex>
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

class ProfilerRoute : public CrtpRoute<ProfilerRoute> {
       public:
	ProfilerRoute();

	virtual void Register(App& app) override;
	virtual const char* route() const noexcept override { return route_; }
	virtual const char* name() const noexcept override { return name_; }

	pcw_crtp_route_def_impl__(int);
	Response impl(HttpGet, const Request& req, int bid) const;
	Response impl(HttpPost, const Request& req, int bid) const;

       private:
	using Mutex = std::mutex;
	using MutexPtr = std::shared_ptr<Mutex>;
	using Lock = std::lock_guard<Mutex>;
	using Jobs = std::unordered_map<int, std::future<Maybe<Profile>>>;
	using JobsPtr = std::shared_ptr<Jobs>;
	using Result = Maybe<Profile>;

	bool is_running_job_id(int id) const noexcept {
		return jobs_->count(id);
	}
	ConstBookSptr get_book(const Request& req, int bid) const;
	ProfilerUptr get_profiler(ConstBookSptr book) const;
	Response handle_new_profile(const Request& req,
				    const Result& res) const;

	static const char *route_, *name_;
	MutexPtr mutex_;
	JobsPtr jobs_;
};
}

#endif  // profiler_ProfilerRoute_hpp__
