#include <crow.h>
#include <cppconn/connection.h>
#include "core/util.hpp"
#include "core/App.hpp"
#include "Config.hpp"

using namespace profiler;

////////////////////////////////////////////////////////////////////////////////
static const char*
do_plugin(const std::string& p, pcw::App& app) noexcept
{
	try {
		auto config = std::make_unique<Config>(p, app.config());
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

