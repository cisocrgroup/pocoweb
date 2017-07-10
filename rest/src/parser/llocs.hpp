#ifndef pcw_llocs_hpp__
#define pcw_llocs_hpp__

#include <boost/filesystem/path.hpp>

namespace pcw {
using Path = boost::filesystem::path;
Path get_llocs_from_png(const Path& png);
}

#endif  // pcw_llocs_hpp__
