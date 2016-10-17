#include "crow.h"
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
	CROW_ROUTE(app, API_VERSION_ROUTE)(*this);
}

////////////////////////////////////////////////////////////////////////////////
crow::response
ApiVersion::operator()() const noexcept
{
	CROW_LOG_INFO << route() << ": " << PCW_API_VERSION;
	return crow::response(PCW_API_VERSION);
}
