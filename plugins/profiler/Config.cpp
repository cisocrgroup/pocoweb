#include "core/Config.hpp"
#include "Config.hpp"

using namespace profiler;

////////////////////////////////////////////////////////////////////////////////
std::unique_ptr<const Config> Config::INSTANCE_ = nullptr;

////////////////////////////////////////////////////////////////////////////////
Config::Config(const std::string& name, const pcw::Config& config)
	: backend_(config.plugins[name].get<std::string>("backend"))
	, exe_(config.plugins[name].get<std::string>("exe"))
	, name_(name)
	, local_(config.plugins[name].get<bool>("local"))
{
}

////////////////////////////////////////////////////////////////////////////////
void
Config::setup(const std::string& name, const pcw::Config& config)
{
	INSTANCE_.reset(new Config(name, config));
}
