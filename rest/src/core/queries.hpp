#ifndef PCW_QUERIES_HPP__
#define PCW_QUERIES_HPP__

#include <boost/optional.hpp>
namespace crow {
class query_string;
}

namespace pcw {
using Query = crow::query_string;
template <class T>
boost::optional<T> query_get(const Query& q, const char* key);
bool query_get(const Query& q, const char* key, bool& out);
bool query_get(const Query& q, const char* key, int& out);
bool query_get(const Query& q, const char* key, double& out);
bool query_get(const Query& q, const char* key, std::string& out);
}

////////////////////////////////////////////////////////////////////////////////
template <class T>
inline boost::optional<T> pcw::query_get(const Query& q, const char* key) {
	T t;
	if (not query_get(q, key, t)) {
		return {};
	}
	return t;
}

#endif  // PCW_QUERIES_HPP__
