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
#pragma GCC diagnostic pop
