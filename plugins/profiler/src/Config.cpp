#include "core/Config.hpp"
#include "Config.hpp"

using namespace profiler;

////////////////////////////////////////////////////////////////////////////////
Config::Config(const std::string& name, const pcw::Config& config)
	: backend_(config.plugins[name].get<std::string>("backend"))
	, exe_(config.plugins[name].get<std::string>("exe"))
	, name_(name)
	, local_(config.plugins[name].get<bool>("local"))
{
}

