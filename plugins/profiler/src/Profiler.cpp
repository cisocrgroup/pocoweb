#include "core/Profile.hpp"
#include "Profiler.hpp"

using namespace profiler;

////////////////////////////////////////////////////////////////////////////////
pcw::Profile
Profiler::profile() noexcept
{
	// try {
		return do_profile();
	// } catch (const std::exception& e) {

	// } catch (...) {

	// }
}
