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
	Lock lock(*mutex_);
	const auto n = get_config().profiler.jobs;
	LockedSession session(must_find_session(req));
	auto conn = must_get_connection();
	session->assert_permission(conn, bid, Permissions::Write);
	auto project = session->must_find(conn, bid);
	if (jobs_->size() < n) {
		if (not jobs_->count(bid)) {
			// jobs_->insert(obj.data().origin().id());
			// std::thread worker(
			//     [&]() { profile(this, std::move(obj)); });
			// worker.detach();
			// profile(this, std::move(obj));
		}
		// return accepted();
	}
	// return service_not_available();
	return not_implemented();
}

////////////////////////////////////////////////////////////////////////////////
ProfilerUptr ProfilerRoute::get_profiler(ConstBookSptr book) const {
	if (get_config().profiler.local) {
		return std::make_unique<LocalProfiler>(book, get_config());
	} else {
		return std::make_unique<RemoteProfiler>(book);
	}
}

// ////////////////////////////////////////////////////////////////////////////////
// void ProfilerRoute::profile(const ProfilerRoute* that,
// 			    ProjectSessionObject obj) noexcept {
// 	try {
// 		// remove id from jobs before returning
// 		const auto id = obj.data().id();
// 		ScopeGuard sg([that, id]() {
// 			assert(that);
// 			assert(that->mutex_);
// 			// Lock lock(*that->mutex_);
// 			that->jobs_->erase(id);
// 		});
// 		CROW_LOG_DEBUG << "(ProfilerRoute) profiling ...";
// 		auto profiler =
// 		    that->get_profiler(std::dynamic_pointer_cast<const Book>(
// 			obj.data().origin().shared_from_this()));
// 		auto profile = profiler->profile();
// 		insert_profile(profile.get(), obj,
// 			       that->config().profiler.min_weight);
// 		CROW_LOG_DEBUG << "(ProfilerRoute) done profiling";
// 	} catch (const std::exception& e) {
// 		CROW_LOG_ERROR << "(ProfilerRoute) Could not profile book id: "
// 			       << obj.data().origin().id() << ": " << e.what();
// 	} catch (...) {
// 		CROW_LOG_ERROR << "(ProfilerRoute) Unkown error";
// 	}
// }
//
// ////////////////////////////////////////////////////////////////////////////////
// void ProfilerRoute::insert_profile(const Profile& profile,
// 				   ProjectSessionObject& obj,
// 				   double min_weight) {
// 	using namespace sqlpp;
// 	tables::Profiles profiles;
// 	tables::Errortokens errortokens;
// 	tables::Suggestions suggestions;
// 	const auto id = obj.data().origin().id();
// 	const auto ts = static_cast<uintmax_t>(std::time(nullptr));
// 	MysqlCommiter commiter(obj.conn());
// 	auto row = obj.conn().db()(select(profiles.bookid, profiles.timestamp)
// 				       .from(profiles)
// 				       .where(profiles.bookid == id));
// 	// new profile with a new timestamp
// 	if (row.empty()) {
// 		obj.conn().db()(insert_into(profiles).set(
// 		    profiles.bookid = id, profiles.timestamp = ts));
// 	} else {
// 		obj.conn().db()(update(profiles)
// 				    .set(profiles.timestamp = ts)
// 				    .where(profiles.bookid == id));
// 	}
// 	// remove old profiles (if possible)
// 	obj.conn().db()(
// 	    remove_from(errortokens).where(errortokens.bookid == id));
// 	obj.conn().db()(
// 	    remove_from(suggestions).where(suggestions.bookid == id));
// 	// insert new profile
// 	std::set<std::wstring> seen;
// 	for (const auto& s : profile.suggestions()) {
// 		if (seen.count(s.first.wcor_lc())) continue;
// 		seen.insert(s.first.wcor_lc());
// 		const auto etid = static_cast<int>(seen.size());
// 		obj.conn().db()(
// 		    insert_into(errortokens)
// 			.set(errortokens.errortokenid = etid,
// 			     errortokens.bookid = id,
// 			     errortokens.errortoken = s.first.cor_lc()));
// 		for (const auto& c : s.second) {
// 			if (c.weight() < min_weight) continue;
// 			if (c.lev() <= 0) continue;
// 			CROW_LOG_DEBUG << "(ProfilerRoute) [" << s.first.cor()
// 				       << "] " << c.cor() << ": "
// 				       << c.explanation_string() << " ("
// 				       << c.weight() << ")";
// 			CROW_LOG_DEBUG << "(ProfilerRoute) inserting: " << id
// 				       << "-" << etid << "-" << c.cor() << "-"
// 				       << c.weight() << "-" << c.lev();
// 			obj.conn().db()(
// 			    insert_into(suggestions)
// 				.set(suggestions.bookid = id,
// 				     suggestions.errortokenid = etid,
// 				     suggestions.suggestion = c.cor(),
// 				     suggestions.weight = c.weight(),
// 				     suggestions.distance = c.lev()));
// 		}
// 	}
// 	commiter.commit();
// }
