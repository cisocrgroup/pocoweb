#ifndef pcw_Config_hpp__
#define pcw_Config_hpp__

#include <boost/property_tree/ptree.hpp>
#include <string>
#include <unordered_map>

namespace pcw {
using Ptree = boost::property_tree::ptree;

struct Config {
  static Config empty();
  static Config load(const std::string &filename);
  static Config load(std::istream &is);
  void setup_logging() const;

  const struct Db {
    const std::string user, host, pass, name;
    const int connections;
    const bool debug;
  } db;

  const struct Daemon { const std::string host, projectdir; } daemon;

  const struct Log { const int level; } log;

  void LOG() const;
};
} // namespace pcw

#endif // pcw_Config_hpp__
