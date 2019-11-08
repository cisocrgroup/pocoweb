#ifndef pcw_rapidjson_hpp__
#define pcw_rapidjson_hpp__

#ifndef RAPIDJSON_HAS_CXX11_RVALUE_REFS
#define RAPIDJSON_HAS_CXX11_RVALUE_REFS 1
#endif // RAPIDJSON_HAS_CXX11_RVALUE_REFS

#ifndef RAPIDJSON_HAS_STDSTRING
#define RAPIDJSON_HAS_STDSTRING 1
#endif // RAPIDJSON_HAS_STDSTRING 1

#include <cmath>
#include <limits>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

namespace pcw {
////////////////////////////////////////////////////////////////////////////////
template <class OS> void fixed_double(rapidjson::Writer<OS> &w, double val) {
  if (std::isnan(val)) {
    val = 0;
  } else if (std::isinf(val) and val > 0) {
    val = std::numeric_limits<double>::max();
  } else if (std::isinf(val) and val < 0) {
    val = std::numeric_limits<double>::min();
  }
  w.Double(val);
}
} // namespace pcw
#endif // pcw_rapidjson_hpp__
