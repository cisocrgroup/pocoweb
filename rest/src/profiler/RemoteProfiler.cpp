#include "RemoteProfiler.hpp"
#include "Profile.hpp"
#include "utils/Error.hpp"

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsuggest-attribute=noreturn"
Profile RemoteProfiler::do_profile() {
	THROW(NotImplemented, "RemoteProfiler::do_profile");
}

////////////////////////////////////////////////////////////////////////////////
std::vector<std::string> RemoteProfiler::do_languages() {
	THROW(NotImplemented, "RemoteProfiler::do_languages");
}
#pragma GCC diagnostic pop
