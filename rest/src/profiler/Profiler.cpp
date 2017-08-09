#include "Profile.hpp"
#include "Profiler.hpp"
#include "utils/Maybe.hpp"

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
pcw::Maybe<Profile> Profiler::profile() noexcept {
	return pcw::Maybe<pcw::Profile>::from_lambda(
	    [this]() { return this->do_profile(); });
}
