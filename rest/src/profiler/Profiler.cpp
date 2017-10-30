#include "Profile.hpp"
#include "Profiler.hpp"
#include "utils/Maybe.hpp"

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
pcw::Maybe<Profile> Profiler::profile() noexcept {
	return pcw::Maybe<pcw::Profile>::from_lambda(
	    [this]() { return this->do_profile(); });
}

////////////////////////////////////////////////////////////////////////////////
pcw::Maybe<std::vector<std::string>> Profiler::languages() noexcept {
	return pcw::Maybe<std::vector<std::string>>::from_lambda(
	    [this]() { return this->do_languages(); });
}
