#include <crow/app.h>
#include "core/Database.hpp"
#include "core/App.hpp"
#include "core/Book.hpp"
#include "core/Maybe.hpp"
#include "core/Profile.hpp"
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
ProfilerRoute::ProfilerRoute(ConfigPtr config)
	: mutex_(std::make_unique<Mutex>())
	, config_(std::move(config))
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
	Lock lock(*mutex_);
	return not_implemented();
}

////////////////////////////////////////////////////////////////////////////////
ProfilerRoute::Response
ProfilerRoute::impl(HttpPost, const Request& req, int bid) const
{
	// query book
	auto db = database(req);
	auto view = find(db, bid);
	if (not view)
		THROW(pcw::NotFound, "Cannot find book id: ", bid);
	auto book = view->origin().shared_from_this();
	if (not book)
		THROW(pcw::NotFound, "Cannot find book id: ", bid);
	assert(book);
	assert(book->is_book());

	Lock lock(*mutex_);
	if (not jobs_->count(book->id())) {
		jobs_->emplace(book->id(), std::async(std::launch::async, [this]() {
			auto profiler = get_profiler();
			return profiler->profile();
		}));
	}
	return accepted();
}

////////////////////////////////////////////////////////////////////////////////
ProfilerPtr
ProfilerRoute::get_profiler() const
{
	if (config_->local())
		return std::make_unique<LocalProfiler>(config_);
	else
		return std::make_unique<RemoteProfiler>(config_);
}
