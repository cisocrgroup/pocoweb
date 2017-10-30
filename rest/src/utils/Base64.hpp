#ifndef PCW_BASE_64_HPP__
#define PCW_BASE_64_HPP__
#include <string>
namespace pcw {
namespace base64 {
std::string encode(const std::string& bindata);
std::string decode(const std::string& bindata);
}
}
#endif  // PCW_BASE_64_HPP__
