#ifndef pcw_Config_hpp__
#define pcw_Config_hpp__

#include <boost/property_tree/ptree.hpp>
#include <string>
#include <unordered_map>

namespace pcw {
using Ptree = boost::property_tree::ptree;

struct Config
{
  static Config empty();
  static Config load(const std::string& filename);
  static Config load(std::istream& is);
  void setup_logging() const;

  const struct Db
  {
    const std::string user, host, pass, name;
    const int connections;
    const bool debug;
  } db;

  const struct Daemon
  {
    const std::string host, user, basedir, projectdir, pidfile;
    const int port, threads;
    const bool detach;
  } daemon;

  const struct Log
  {
    const int level;
  } log;

  const struct Profiler
  {
    const std::string langdir, exe;
    const double min_weight;
    const size_t jobs;
    const bool debug;
  } profiler;

  void LOG() const;
};
}

#endif // pcw_Config_hpp__
