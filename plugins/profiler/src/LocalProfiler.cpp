#include <chrono>
#include <thread>
#include <crow/logging.h>
#include "core/Error.hpp"
#include "core/Profile.hpp"
#include "LocalProfiler.hpp"

using namespace profiler;

////////////////////////////////////////////////////////////////////////////////
pcw::Profile
LocalProfiler::do_profile()
{
	using namespace std::chrono_literals;
	auto sleeping_time = 10s;
	CROW_LOG_DEBUG << "(LocalProfiler) going to sleep for 10s";
	std::this_thread::sleep_for(sleeping_time);
	THROW(pcw::NotImplemented, "Not implemented LocalProfiler::do_profile");
}
