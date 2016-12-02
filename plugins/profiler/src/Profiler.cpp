#include "core/Maybe.hpp"
#include "core/Profile.hpp"
#include "Profiler.hpp"

using namespace profiler;

////////////////////////////////////////////////////////////////////////////////
pcw::Maybe<pcw::Profile>
Profiler::profile() noexcept
{
	return pcw::Maybe<pcw::Profile>::from_lambda([this](){
		return this->do_profile();
	});
}
