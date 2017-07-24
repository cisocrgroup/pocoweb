#include "profiler/Profile.hpp"
#include <crow/app.h>
#include <chrono>
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
    : mutex_(std::make_shared<std::mutex>()),
      jobs_(std::make_shared<std::set<int>>()) {}

////////////////////////////////////////////////////////////////////////////////
ProfilerRoute::~ProfilerRoute() noexcept {
	CROW_LOG_DEBUG << "(ProfilerRoute) waiting for unfinished jobs ... ";
	while (true) {
		Lock lock(*mutex_);
		if (jobs_->empty()) {
			break;
		} else {
			using namespace std::chrono_literals;
			// wait untill all detached threads have finished.
			std::this_thread::sleep_for(2s);
		}
	}
	CROW_LOG_DEBUG << "(ProfilerRoute) all jobs have finished.";
}

////////////////////////////////////////////////////////////////////////////////
void ProfilerRoute::Register(App& app) {
	CROW_ROUTE(app, PROFILER_ROUTE_ROUTE)
	    .methods("GET"_method, "POST"_method)(*this);
}

////////////////////////////////////////////////////////////////////////////////
ProfilerRoute::Response ProfilerRoute::impl(HttpGet, const Request& req,
					    int bid) const {
	return not_implemented();
}

////////////////////////////////////////////////////////////////////////////////
ProfilerRoute::Response ProfilerRoute::impl(HttpPost, const Request& req,
					    int bid) const {
	// query book
	CROW_LOG_DEBUG << "(ProfilerRoute) order profile for project id: "
		       << bid;
	const auto book = get_book(req, bid);
	assert(book);
	CROW_LOG_DEBUG << "(ProfilerRoute) profiling book id: " << book->id();
	Lock lock(*mutex_);
	const auto n = config().profiler.jobs;
	if (jobs_->size() < n) {
		if (not jobs_->count(book->id())) {
			jobs_->insert(book->id());
			std::thread worker(
			    [this, book]() { profile(this, book); });
			worker.detach();
		}
		return accepted();
	}
	// return service_not_available();
	return not_implemented();
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

////////////////////////////////////////////////////////////////////////////////
void ProfilerRoute::profile(const ProfilerRoute* that, ConstBookSptr book) {
	try {
		// remove id from jobs before returning
		const auto id = book->id();
		ScopeGuard sg([that, id]() {
			assert(that);
			assert(that->mutex_);
			Lock lock(*that->mutex_);
			that->jobs_->erase(id);
		});
		auto profiler = that->get_profiler(book);
		auto profile = profiler->profile();
		CROW_LOG_DEBUG << "(ProfilerRoute) done profiling";
		for (const auto& s : profile.get().suggestions()) {
			CROW_LOG_DEBUG << "(ProfilerRoute) [" << s.token.cor()
				       << "] " << s.cand.cor() << ": "
				       << s.cand.explanation_string() << " ("
				       << s.cand.weight() << ")";
		}
	} catch (const std::exception& e) {
		CROW_LOG_ERROR << "(ProfilerRoute) Could not profile book id: "
			       << book->id() << ": " << e.what();
	}
}
