#include <crow.h>
#include <cppconn/connection.h>
#include "core/util.hpp"
#include "core/App.hpp"
#include "core/Config.hpp"
#include "Config.hpp"
#include "ProfilerRoute.hpp"

using namespace profiler;

////////////////////////////////////////////////////////////////////////////////
static const char*
do_plugin(const std::string& p, pcw::App& app) noexcept
{
	try {
		app.config().setup_logging();
		Config::setup(p, app.config());
		auto route = std::make_unique<ProfilerRoute>();
		app.Register(std::move(route));
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

