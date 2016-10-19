#include <cppconn/connection.h>
#include <crow.h>
#include "Sessions.hpp"
#include "LoggedIn.hpp"

using namespace pcw;

#define LOGGED_IN_ROUTE "/logged-in"

////////////////////////////////////////////////////////////////////////////////
const char* LoggedIn::route_ = LOGGED_IN_ROUTE;

////////////////////////////////////////////////////////////////////////////////
const char* LoggedIn::name_ = "LoggedIn";

////////////////////////////////////////////////////////////////////////////////
void 
LoggedIn::Register(App& app) 
{
	CROW_ROUTE(app, LOGGED_IN_ROUTE)(*this);
}

////////////////////////////////////////////////////////////////////////////////
crow::response 
LoggedIn::operator()(const crow::request& request) const
{
	auto session = this->session(request);
	return session ? ok() : forbidden();
}
