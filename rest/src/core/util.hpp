#ifndef pcw_util_hpp__
#define pcw_util_hpp__

#include <stdexcept>

namespace pcw {
	const char* what(const std::exception& e) noexcept;
}

#endif // pcw_util_hpp__
