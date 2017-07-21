#include "profiler/Profile.hpp"
#include <crow/app.h>
#include "ProfilerRoute.hpp"
#include "core/App.hpp"
#include "core/Book.hpp"
#include "core/Config.hpp"
#include "core/Session.hpp"
#include "profiler/LocalProfiler.hpp"
#include "profiler/RemoteProfiler.hpp"
#include "utils/Maybe.hpp"

#define PROFILER_ROUTE_ROUTE "/profile/<int>"

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
const char* ProfilerRoute::route_ = PROFILER_ROUTE_ROUTE;
const char* ProfilerRoute::name_ = "ProfilerRoute";

////////////////////////////////////////////////////////////////////////////////
ProfilerRoute::ProfilerRoute()
    : mutex_(std::make_unique<Mutex>()), jobs_(std::make_shared<Jobs>()) {}

////////////////////////////////////////////////////////////////////////////////
void ProfilerRoute::Register(App& app) {
	CROW_ROUTE(app, PROFILER_ROUTE_ROUTE)
	    .methods("GET"_method, "POST"_method)(*this);
}

////////////////////////////////////////////////////////////////////////////////
ProfilerRoute::Response ProfilerRoute::impl(HttpGet, const Request& req,
					    int bid) const {
	// query book
	auto book = get_book(req, bid);
	assert(book);

	Lock lock(*mutex_);
	for (const auto& x : *jobs_) {
		CROW_LOG_DEBUG << "(ProfilerRoute) job id: " << x.first;
	}
	if (is_running_job_id(book->id())) {
		CROW_LOG_DEBUG << "(ProfilerRoute) running job: " << book->id();
		using namespace std::chrono_literals;
		auto amount = 2s;
		auto future = jobs_->find(bid);
		assert(future != end(*jobs_));
		auto status = future->second.wait_for(amount);
		if (status == std::future_status::ready) {  // job is done
			CROW_LOG_INFO
			    << "(ProfilerRoute) Job id: " << book->id()
			    << " done";
			auto res = future->second.get();  // should not block
			jobs_->erase(future);		  // remove job
			return handle_new_profile(req, res);
		} else {  // job is not done yet
			CROW_LOG_INFO
			    << "(ProfilerRoute) Job id: " << book->id()
			    << " not done yet";
			return accepted();
		}
	} else {  // no such job
		CROW_LOG_DEBUG << "(ProfilerRoute) not a running job: "
			       << book->id();
		return not_found();
	}
}

////////////////////////////////////////////////////////////////////////////////
ProfilerRoute::Response ProfilerRoute::handle_new_profile(
    const Request& req, const Result& res) const {
	auto profile = std::move(res.get());
	for (const auto& s : profile.suggestions()) {
		CROW_LOG_DEBUG << "(ProfilerRoute) Suggestion: " << s.cand.cor()
			       << ": " << s.cand.explanation_string();
	}
	return ok();
}

////////////////////////////////////////////////////////////////////////////////
ProfilerRoute::Response ProfilerRoute::impl(HttpPost, const Request& req,
					    int bid) const {
	// query book
	CROW_LOG_DEBUG << "(ProfilerRoute) order profile for project id: "
		       << bid;
	auto book = get_book(req, bid);
	assert(book);
	CROW_LOG_DEBUG << "(ProfilerRoute) profiling book id: " << book->id();

	Lock lock(*mutex_);
	if (not is_running_job_id(book->id())) {
		CROW_LOG_DEBUG << "(ProfilerRoute) new job: " << book->id();
		jobs_->emplace(book->id(),
			       std::async(std::launch::async, [&]() {
				       auto profiler = get_profiler(book);
				       return profiler->profile();
			       }));
	} else {
		CROW_LOG_DEBUG << "(ProfilerRoute) job already running: "
			       << book->id();
	}
	return accepted();
}

////////////////////////////////////////////////////////////////////////////////
ProfilerUptr ProfilerRoute::get_profiler(ConstBookSptr book) const {
	if (config().profiler.local) {
		return std::make_unique<LocalProfiler>(book, config());
	} else {
		return std::make_unique<RemoteProfiler>(book);
	}
}

////////////////////////////////////////////////////////////////////////////////
ConstBookSptr ProfilerRoute::get_book(const Request& req, int bid) const {
	CROW_LOG_DEBUG << "(ProfilerRoute) getting session object";
	auto obj = this->new_project_session(req, bid);
	CROW_LOG_DEBUG << "(ProfilerRoute) got session object";
	const auto book = std::dynamic_pointer_cast<const Book>(
	    obj.data().origin().shared_from_this());
	CROW_LOG_DEBUG << "(ProfilerRoute) getting book";
	if (not book)
		THROW(Error, "Cannot determine origin of project id: ", bid);
	CROW_LOG_DEBUG << "(ProfilerRoute) got book";
	obj.session().assert_permission(obj.conn(), book->id(),
					Permissions::Write);
	CROW_LOG_DEBUG << "(ProfilerRoute) returning book";
	return book;
}
