#include "crow.h"
#include "core/jsonify.hpp"
#include "core/App.hpp"
#include "ApiVersion.hpp"

#define API_VERSION_ROUTE "/api-version"

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
const char* ApiVersion::route_ = API_VERSION_ROUTE;
const char* ApiVersion::name_ = "ApiVersion";

////////////////////////////////////////////////////////////////////////////////
void
ApiVersion::Register(App& app)
{
	CROW_ROUTE(app, API_VERSION_ROUTE).methods("GET"_method)(*this);
}

////////////////////////////////////////////////////////////////////////////////
Route::Response
ApiVersion::operator()() const noexcept
{
	CROW_LOG_INFO << route() << ": " << pcw::App::version_str();
	Json j;
	j["version"] = pcw::App::version_str();
	return Response(j);
}
