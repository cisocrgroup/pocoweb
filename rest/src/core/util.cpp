#include <cstring>
#include "util.hpp"

#ifndef PCW_WHAT_LEN
#define PCW_WHAT_LEN 1024
#endif // PCW_WHAT_LEN

// using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
const char*
pcw::what(const std::exception& e) noexcept
{
	static char buffer[PCW_WHAT_LEN];
	return strncpy(buffer, e.what(), PCW_WHAT_LEN);
}
