#include <chrono>
#include <thread>
#include <crow/logging.h>
#include "core/Book.hpp"
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
	CROW_LOG_INFO << "(LocalProfiler) Profiling book id: " << book().id();
	std::this_thread::sleep_for(sleeping_time);
	CROW_LOG_INFO << "(LocalProfiler) Done profiling book id: " << book().id();
	THROW(pcw::NotImplemented, "Not implemented LocalProfiler::do_profile");
}
