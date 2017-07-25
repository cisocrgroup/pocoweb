#include "profiler/Profile.hpp"
#include <crow/app.h>
#include <chrono>
#include "ProfilerRoute.hpp"
#include "core/App.hpp"
#include "core/Book.hpp"
#include "core/Config.hpp"
#include "core/Session.hpp"
#include "database/Tables.h"
#include "profiler/LocalProfiler.hpp"
#include "profiler/RemoteProfiler.hpp"
#include "utils/Maybe.hpp"

#define PROFILER_ROUTE_ROUTE "/books/<int>/profile"

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
const char* ProfilerRoute::route_ = PROFILER_ROUTE_ROUTE;
const char* ProfilerRoute::name_ = "ProfilerRoute";

////////////////////////////////////////////////////////////////////////////////
// cannot initialize jobs here, since get_config() is not set after all routes
// have been registered.
ProfilerRoute::ProfilerRoute()
    : mutex_(std::make_shared<std::mutex>()), jobs_() {}

////////////////////////////////////////////////////////////////////////////////
ProfilerRoute::~ProfilerRoute() noexcept {
	CROW_LOG_DEBUG << "(ProfilerRoute) waiting for unfinished jobs ... ";
	std::lock_guard<std::mutex> lock(*mutex_);
	for (auto& job : *jobs_) {
		if (job.t.joinable()) {
			job.t.join();
		}
	}
	CROW_LOG_DEBUG << "(ProfilerRoute) all jobs have finished.";
}

////////////////////////////////////////////////////////////////////////////////
void ProfilerRoute::Register(App& app) {
	// setup jobs (Register() is called after each route has been set up).
	// must be called before the route is registered.
	jobs_ = std::make_shared<std::vector<Job>>(get_config().profiler.jobs);
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
	LockedSession session(must_find_session(req));
	auto conn = must_get_connection();
	session->assert_permission(conn, bid, Permissions::Write);
	const auto project = session->must_find(conn, bid);
	const auto book = std::dynamic_pointer_cast<const Book>(
	    project->origin().shared_from_this());
	if (not book) {
		THROW(Error,
		      "(ProfilerRoute) Cannot get origin of "
		      "book id: ",
		      bid);
	}
	// lock the jobs
	std::lock_guard<std::mutex> lock(*mutex_);
	auto job = find_free_job(book->id());
	// no more threads possible
	if (not job) return service_not_available();
	// there is already a running job
	if (job->running) return accepted();
	// wait for the job's thread
	if (job->t.joinable()) job->t.join();
	job->id = book->id();
	job->t = std::thread([=]() {
		job->running = true;
		// profile does not throw!
		profile(this, book);
		job->running = false;
	});
	return accepted();
}

////////////////////////////////////////////////////////////////////////////////
ProfilerUptr ProfilerRoute::get_profiler(ConstBookSptr book) const {
	if (get_config().profiler.local) {
		return std::make_unique<LocalProfiler>(book, get_config());
	} else {
		return std::make_unique<RemoteProfiler>(book);
	}
}

////////////////////////////////////////////////////////////////////////////////
ProfilerRoute::Job* ProfilerRoute::find_free_job(int id) const noexcept {
	// jobs must be locked!
	// search for already running job;
	for (auto& job : *jobs_) {
		if (job.id == id and not job.running) {
			return &job;
		}
	}
	for (auto& job : *jobs_) {
		if (not job.running) {
			return &job;
		}
	}
	return nullptr;
}

////////////////////////////////////////////////////////////////////////////////
void ProfilerRoute::profile(const ProfilerRoute* that,
			    ConstBookSptr book) noexcept {
	try {
		CROW_LOG_DEBUG << "(ProfilerRoute) profiling ...";
		auto profiler = that->get_profiler(book);
		auto profile = profiler->profile();
		insert_profile(that, profile.get());
		CROW_LOG_DEBUG << "(ProfilerRoute) done profiling";
	} catch (const std::exception& e) {
		CROW_LOG_ERROR << "(ProfilerRoute) Could not profile book id: "
			       << book->id() << ": " << e.what();
	} catch (...) {
		CROW_LOG_ERROR << "(ProfilerRoute) Unkown error";
	}
}

////////////////////////////////////////////////////////////////////////////////
void ProfilerRoute::insert_profile(const ProfilerRoute* that,
				   const Profile& profile) {
	using namespace sqlpp;
	tables::Profiles profiles;
	tables::Errortokens errortokens;
	tables::Suggestions suggestions;
	auto conn = that->must_get_connection();
	const auto id = profile.book().id();
	const auto ts = static_cast<uintmax_t>(std::time(nullptr));
	const auto min_weight = that->get_config().profiler.min_weight;
	MysqlCommiter commiter(conn);
	// remove old profiles (if possible)
	conn.db()(remove_from(errortokens).where(errortokens.bookid == id));
	conn.db()(remove_from(suggestions).where(suggestions.bookid == id));
	conn.db()(remove_from(profiles).where(profiles.bookid == id));
	// insert new profile timestamp
	conn.db()(insert_into(profiles).set(profiles.bookid = id,
					    profiles.timestamp = ts));

	// insert new profile
	std::set<std::wstring> seen;
	for (const auto& s : profile.suggestions()) {
		if (seen.count(s.first.wcor_lc())) continue;
		seen.insert(s.first.wcor_lc());
		const auto etid = static_cast<int>(seen.size());
		conn.db()(insert_into(errortokens)
			      .set(errortokens.errortokenid = etid,
				   errortokens.bookid = id,
				   errortokens.errortoken = s.first.cor_lc()));
		for (const auto& c : s.second) {
			if (c.weight() < min_weight) continue;
			if (c.lev() <= 0) continue;
			CROW_LOG_DEBUG << "(ProfilerRoute) [" << s.first.cor()
				       << "] " << c.cor() << ": "
				       << c.explanation_string() << " ("
				       << c.weight() << ")";
			CROW_LOG_DEBUG << "(ProfilerRoute) inserting: " << id
				       << "-" << etid << "-" << c.cor() << "-"
				       << c.weight() << "-" << c.lev();
			conn.db()(insert_into(suggestions)
				      .set(suggestions.bookid = id,
					   suggestions.errortokenid = etid,
					   suggestions.suggestion = c.cor(),
					   suggestions.weight = c.weight(),
					   suggestions.distance = c.lev()));
		}
	}
	commiter.commit();
}
