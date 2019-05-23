#include "queries.hpp"
#include <boost/lexical_cast.hpp>
#define private public // yes we can
#include <crow.h>
#undef private

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
bool pcw::query_get(const Query &q, const char *key, bool &out) {
  const auto p = q.get(key);
  if (not p) {
    return false;
  }
  try {
    out = boost::lexical_cast<bool>(p);
    return true;
  } catch (const boost::bad_lexical_cast &) {
  }
  return false;
}

////////////////////////////////////////////////////////////////////////////////
bool pcw::query_get(const Query &q, const char *key, int &out) {
  const auto p = q.get(key);
  if (not p) {
    return false;
  }
  try {
    out = boost::lexical_cast<int>(p);
    return true;
  } catch (const boost::bad_lexical_cast &) {
  }
  return false;
}

////////////////////////////////////////////////////////////////////////////////
bool pcw::query_get(const Query &q, const char *key, double &out) {
  const auto p = q.get(key);
  if (not p) {
    return false;
  }
  try {
    out = boost::lexical_cast<double>(p);
    return true;
  } catch (const boost::bad_lexical_cast &) {
  }
  return false;
}

////////////////////////////////////////////////////////////////////////////////
bool pcw::query_get(const Query &q, const char *key, std::string &out) {
  const auto p = q.get(key);
  if (not p) {
    return false;
  }
  out = p;
  return true;
}

////////////////////////////////////////////////////////////////////////////////
bool pcw::query_get(const Query &q, const char *key,
                    std::vector<std::string> &out) {
  out.clear();
  int count = 0;
  while (true) {
    const char *val = crow::qs_k2v(key, q.key_value_pairs_.data(),
                                   q.key_value_pairs_.size(), count++);
    if (not val) {
      break;
    }
    std::cout << "PUSHING BACK: " << val << "\n";
    out.push_back(std::string(val));
  }
  // const auto ps = q.get_list(key);
  //
  // for (const char *val : ps) {
  //
  //   out.push_back(std::string(val));
  // }
  std::cout << "len out: " << out.size() << "\n";
  return not out.empty();
}
// element = qs_k2v(plus.c_str(), key_value_pairs_.data(),
// key_value_pairs_.size(), count++); inline char * qs_k2v(const char * key,
// char * const * qs_kv, int qs_kv_size, int nth = 0)
