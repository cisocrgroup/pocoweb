#include <crow/app.h>
#include "core/App.hpp"
#include "Config.hpp"
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
	Lock lock(*mutex_);
	return not_implemented();
}
