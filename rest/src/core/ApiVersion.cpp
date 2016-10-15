#include "crow.h"
#include "ApiVersion.hpp"

#define ROUTE "/api-version"

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
const char* ApiVersion::route_ = ROUTE;

////////////////////////////////////////////////////////////////////////////////
void
ApiVersion::Register(App& app)
{
	CROW_ROUTE(app, ROUTE)(*this);
}

////////////////////////////////////////////////////////////////////////////////
crow::response
ApiVersion::operator()() const noexcept
{
	CROW_LOG_INFO << route() << ": " << PCW_API_VERSION;
	return crow::response(PCW_API_VERSION);
}
