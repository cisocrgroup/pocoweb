#include "crow.h"
#include "util.hpp"
#include "App.hpp"

////////////////////////////////////////////////////////////////////////////////
class PluginRoute: public pcw::Route {
public:
	virtual ~PluginRoute() noexcept override = default;
	virtual void Register(App& app) override;
	virtual const char* name() const noexcept override {return "example";}
	virtual const char* route() const noexcept override {return "/plugin/<int>/<int>";}
	crow::response operator()(int a, int b) const;

};

////////////////////////////////////////////////////////////////////////////////
crow::response
PluginRoute::operator()(int a, int b) const
{
	CROW_LOG_INFO << "(example) got a = " << a << ", b = " << b;
	return crow::response(200);
}

////////////////////////////////////////////////////////////////////////////////
void
PluginRoute::Register(App& app)
{
	CROW_ROUTE(app, "/plugin/<int>/<int>")(*this);
}

////////////////////////////////////////////////////////////////////////////////
const char*
do_plugin(const std::string& p, pcw::App& app) noexcept
{
	try {
		CROW_LOG_INFO << "(example) setting: "
			      << app.config.plugins[p].get<std::string>("setting");
		CROW_LOG_INFO << "(example) sutting: "
			      << app.config.plugins[p].get<std::string>("sutting");
		app.routes.push_back(std::make_unique<PluginRoute>());
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

