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
    : mutex_(std::make_shared<std::mutex>()),
      workers_(std::make_shared<std::vector<Worker>>(10)) {}

////////////////////////////////////////////////////////////////////////////////
ProfilerRoute::~ProfilerRoute() noexcept {
	CROW_LOG_DEBUG << "(ProfilerRoute) stopping workers ...";
	CROW_LOG_DEBUG << "(ProfilerRoute) all workers have stopped";
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
	auto book = get_book(req, bid);
	assert(book);
	CROW_LOG_DEBUG << "(ProfilerRoute) profiling book id: " << book->id();
	auto profiler = get_profiler(book);
	std::lock_guard<std::mutex> lock(*mutex_);
	for (auto& worker : *workers_) {
		if (not worker.running) {
			worker.running = true;
			worker.thread =
			    std::thread([&]() { profile(this, worker, book); });
			return accepted();
		}
	}
	return not_implemented();
}

////////////////////////////////////////////////////////////////////////////////
void ProfilerRoute::profile(const ProfilerRoute* that, Worker& worker,
			    ConstBookSptr book) {
	CROW_LOG_DEBUG << "(ProfilerRoute) starting to profile ...";
	auto profiler = that->get_profiler(book);
	auto profile = profiler->profile();
	CROW_LOG_DEBUG << "(ProfilerRoute) done profiling";
	try {
		for (const auto& s : profile.get().suggestions()) {
			CROW_LOG_DEBUG << "(ProfilerRoute) [" << s.token.cor()
				       << "] " << s.cand.cor() << ": "
				       << s.cand.explanation_string();
		}
	} catch (const std::exception& e) {
		CROW_LOG_ERROR << "(ProfilerRoute) Could not profile book id: "
			       << book->id() << ": " << e.what();
	}
	worker.running = false;
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
