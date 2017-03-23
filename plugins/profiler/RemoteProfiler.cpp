#include "utils/Error.hpp"
#include "core/Profile.hpp"
#include "RemoteProfiler.hpp"

using namespace profiler;

////////////////////////////////////////////////////////////////////////////////
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsuggest-attribute=noreturn"
pcw::Profile
RemoteProfiler::do_profile()
{
	THROW(pcw::NotImplemented, "RemoteProfiler::do_profile");
}
#pragma GCC diagnostic pop
