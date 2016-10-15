#include "crow.h"
#include "ApiVersion.hpp"

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
const char* ApiVersion::NAME = "ApiVersion";
const char* ApiVersion::ROUTE = "/api-version";

////////////////////////////////////////////////////////////////////////////////
void
ApiVersion::Register(App& app)
{
	CROW_ROUTE(app, "/api-version")(*this);
}

////////////////////////////////////////////////////////////////////////////////
crow::response
ApiVersion::operator()() const noexcept
{
	CROW_LOG_INFO << name() << ": " << PCW_API_VERSION;
	return crow::response(200);
}
