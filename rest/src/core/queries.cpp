#include "queries.hpp"
#include <boost/lexical_cast.hpp>
#define private public // We need to acces Query's private stuff :(
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
    // We need to access the internals of q, since q.get_list(key)
    // does not seem to do what is should.
    const char *val = crow::qs_k2v(key, q.key_value_pairs_.data(),
                                   q.key_value_pairs_.size(), count++);
    if (not val) {
      break;
    }
    out.push_back(std::string(val));
  }
  return not out.empty();
}
