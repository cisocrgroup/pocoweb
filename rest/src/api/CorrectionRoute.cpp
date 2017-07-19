#include "CorrectionRoute.hpp"
#include <crow.h>

#define CORRECTION_ROUTE_ROUTE "/correction"

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
const char* CorrectionRoute::route_ = CORRECTION_ROUTE_ROUTE;
const char* CorrectionRoute::name_ = "CorrectionRoute";

////////////////////////////////////////////////////////////////////////////////
void CorrectionRoute::Register(App& app) {
	CROW_ROUTE(app, CORRECTION_ROUTE_ROUTE).methods("POST"_method)(*this);
}

////////////////////////////////////////////////////////////////////////////////
Route::Response CorrectionRoute::impl(HttpPost, const Request& req) const {
	return internal_server_error();
}
