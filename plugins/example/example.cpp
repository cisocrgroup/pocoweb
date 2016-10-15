#include "crow.h"
#include "util.hpp"
#include "App.hpp"

#define ROUTE "/example/<int>/<int>"

////////////////////////////////////////////////////////////////////////////////
class ExampleRoute: public pcw::Route {
public:
	virtual ~ExampleRoute() noexcept override = default;
	virtual void Register(App& app) override;
	virtual const char* route() const noexcept override {return ROUTE;}
	crow::response operator()(int a, int b) const;

};

////////////////////////////////////////////////////////////////////////////////
crow::response
ExampleRoute::operator()(int a, int b) const
{
	CROW_LOG_INFO << route() << " got a = " << a << ", b = " << b;
	return crow::response(200);
}

////////////////////////////////////////////////////////////////////////////////
void
ExampleRoute::Register(App& app)
{
	CROW_ROUTE(app, ROUTE)(*this);
}

////////////////////////////////////////////////////////////////////////////////
const char*
do_plugin(const std::string& p, pcw::App& app) noexcept
{
	try {
		CROW_LOG_INFO << "(" << p << ") setting: "
			      << app.config.plugins[p].get<std::string>("setting");
		app.routes.push_back(std::make_unique<ExampleRoute>());
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

