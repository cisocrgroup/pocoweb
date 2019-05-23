#include "queries.hpp"
#include <boost/lexical_cast.hpp>
#include <crow.h>

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
  const auto ps = q.get_list(key);
  for (const char *val : ps) {
    out.push_back(std::string(val));
  }
  return not out.empty();
}
