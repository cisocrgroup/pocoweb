#include "crow.h"
#include "util.hpp"
#include "App.hpp"
#include "Config.hpp"

#define EXAMPLE_ROUTE_ROUTE "/example/<int>/<int>"

////////////////////////////////////////////////////////////////////////////////
class ExampleRoute: public pcw::Route {
public:
	virtual ~ExampleRoute() noexcept override = default;
	virtual void Register(App& app) override;
	virtual const char* route() const noexcept override {return EXAMPLE_ROUTE_ROUTE;}
	virtual const char* name() const noexcept override {return "ExampleRoute";}
	crow::response operator()(int a, int b) const;

};

////////////////////////////////////////////////////////////////////////////////
crow::response
ExampleRoute::operator()(int a, int b) const
{
	return not_implemented();
}

////////////////////////////////////////////////////////////////////////////////
void
ExampleRoute::Register(App& app)
{
	CROW_ROUTE(app, EXAMPLE_ROUTE_ROUTE)(*this);
}

////////////////////////////////////////////////////////////////////////////////
const char*
do_plugin(const std::string& p, pcw::App& app) noexcept
{
	try {
		CROW_LOG_INFO << "(" << p << ") setting: "
			      << app.config().plugins[p].get<std::string>("setting");
		app.Register(std::make_unique<ExampleRoute>());
		return nullptr;
	} catch (const std::exception& e) {
		return pcw::what(e);
	}
}

extern "C" {
////////////////////////////////////////////////////////////////////////////////
const char*
plugin(const std::string& p, pcw::App& app)
{
	return do_plugin(p, app);
}
}

