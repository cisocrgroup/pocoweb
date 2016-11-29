#include "crow.h"
#include "core/jsonify.hpp"
#include "core/App.hpp"
#include "VersionRoute.hpp"

#define VERSION_ROUTE_ROUTE "/api-version"

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
const char* VersionRoute::route_ = VERSION_ROUTE_ROUTE;
const char* VersionRoute::name_ = "VersionRoute";

////////////////////////////////////////////////////////////////////////////////
void
VersionRoute::Register(App& app)
{
	CROW_ROUTE(app, VERSION_ROUTE_ROUTE).methods("GET"_method)(*this);
}

////////////////////////////////////////////////////////////////////////////////
Route::Response
VersionRoute::impl(HttpGet, const Request&) const noexcept
{
	CROW_LOG_INFO << route() << ": " << pcw::App::version_str();
	Json j;
	j["version"] = pcw::App::version_str();
	return Response(j);
}
