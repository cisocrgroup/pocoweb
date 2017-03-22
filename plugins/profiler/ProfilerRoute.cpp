#include <cppconn/connection.h>
#include <crow/app.h>
#include "core/Database.hpp"
#include "core/App.hpp"
#include "core/Book.hpp"
#include "utils/Maybe.hpp"
#include "core/Profile.hpp"
#include "core/Session.hpp"
#include "Config.hpp"
#include "LocalProfiler.hpp"
#include "RemoteProfiler.hpp"
#include "ProfilerRoute.hpp"

#define PROFILER_ROUTE_ROUTE "/books/<int>/profile"

using namespace profiler;

////////////////////////////////////////////////////////////////////////////////
const char* ProfilerRoute::route_ = PROFILER_ROUTE_ROUTE;
const char* ProfilerRoute::name_ = "ProfilerRoute";

////////////////////////////////////////////////////////////////////////////////
ProfilerRoute::ProfilerRoute()
	: mutex_(std::make_unique<Mutex>())
	, jobs_(std::make_shared<Jobs>())
{
}

////////////////////////////////////////////////////////////////////////////////
void
ProfilerRoute::Register(App& app)
{
	CROW_ROUTE(app, PROFILER_ROUTE_ROUTE).methods("GET"_method, "POST"_method)(*this);
}

////////////////////////////////////////////////////////////////////////////////
ProfilerRoute::Response
ProfilerRoute::impl(HttpGet, const Request& req, int bid) const
{
	// query book
	auto book = get_book(req, bid);
	assert(book);

	Lock lock(*mutex_);
	if (is_running_job_id(book->id())) {
		using namespace std::chrono_literals;
		auto amount = 2s;
		auto future = jobs_->find(bid);
		assert(future != end(*jobs_));
		auto status = future->second.wait_for(amount);
		if (status == std::future_status::ready) { // job is done
			CROW_LOG_INFO << "(ProfilerRoute) Job id: " << book->id()
				      << " done";
			auto res = future->second.get(); // should not block
			jobs_->erase(future); // remove job
			return handle_new_profile(req, res);
		} else { // job is not done yet
			CROW_LOG_INFO << "(ProfilerRoute) Job id: " << book->id()
				      << " not done yet";
			return accepted();
		}
	} else { // no such job
		return not_implemented();
	}
}

////////////////////////////////////////////////////////////////////////////////
ProfilerRoute::Response
ProfilerRoute::handle_new_profile(const Request& req, const Result& res) const
{
	auto profile = std::move(res.get());
	return not_implemented();
}

////////////////////////////////////////////////////////////////////////////////
ProfilerRoute::Response
ProfilerRoute::impl(HttpPost, const Request& req, int bid) const
{
	// query book
	auto book = get_book(req, bid);
	assert(book);

	Lock lock(*mutex_);
	if (not is_running_job_id(book->id())) {
		jobs_->emplace(book->id(), std::async(std::launch::async, [book]() {
			auto profiler = get_profiler(book);
			return profiler->profile();
		}));
	}
	return accepted();
}

////////////////////////////////////////////////////////////////////////////////
ProfilerUptr
ProfilerRoute::get_profiler(ConstBookSptr book)
{
	if (Config::get().local())
		return std::make_unique<LocalProfiler>(book);
	else
		return std::make_unique<RemoteProfiler>(book);
}

////////////////////////////////////////////////////////////////////////////////
ConstBookSptr
ProfilerRoute::get_book(const Request& req, int bid) const
{
	auto conn = connection();
	auto session = this->session(req);
	assert(conn);
	assert(session);
	pcw::SessionLock sessionlock(*session);
	auto view = session->find(conn, bid);

	if (not view)
		THROW(pcw::NotFound, "Cannot find book id: ", bid);
	auto book = std::dynamic_pointer_cast<const pcw::Book>(
			view->origin().shared_from_this());
	if (not book)
		THROW(pcw::NotFound, "Cannot find origin of book id: ", bid);
	return book;
}
