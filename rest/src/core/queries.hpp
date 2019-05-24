#ifndef PCW_QUERIES_HPP__
#define PCW_QUERIES_HPP__

#include <boost/optional.hpp>
#include <vector>

namespace crow {
class query_string;
}

namespace pcw {
using Query = crow::query_string;
template <class T>
boost::optional<T> query_get(const Query &q, const char *key);
template <class T> T query_get_default(const Query &q, const char *key, T def);
bool query_get(const Query &q, const char *key, bool &out);
bool query_get(const Query &q, const char *key, int &out);
bool query_get(const Query &q, const char *key, double &out);
bool query_get(const Query &q, const char *key, std::string &out);
bool query_get(const Query &q, const char *key, std::vector<std::string> &out);
} // namespace pcw

////////////////////////////////////////////////////////////////////////////////
template <class T>
inline boost::optional<T> pcw::query_get(const Query &q, const char *key) {
  T t;
  if (not query_get(q, key, t)) {
    return {};
  }
  return t;
}

////////////////////////////////////////////////////////////////////////////////
template <class T>
inline T pcw::query_get_default(const Query &q, const char *key, const T def) {
  T t;
  if (not query_get(q, key, t)) {
    return def;
  }
  return t;
}

#endif // PCW_QUERIES_HPP__
