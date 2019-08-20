// #include <boost/log/trivial.hpp>
#include "Config.hpp"
#include "Logger.hpp"
#include "utils/Error.hpp"
#include <boost/algorithm/string/predicate.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <crow/logging.h>
#include <regex>
#include <thread>

#ifndef PCW_CONFIG_EXPANSION_MAX_RUNS
#define PCW_CONFIG_EXPANSION_MAX_RUNS 1000
#endif // PCW_CONFIG_EXPANSION_MAX_RUNS

////////////////////////////////////////////////////////////////////////////////
static std::string get_val(const pcw::Ptree &ptree, const std::string &var) {
  auto val = getenv(var.data());
  if (val)
    return val;
  else
    return ptree.get<std::string>(var);
}

////////////////////////////////////////////////////////////////////////////////
static void expand_variables(pcw::Ptree &ptree) {
  static const std::regex var{R"(\$[({](.+)[})])"};
  std::smatch m;
  size_t runs = 0;
again:
  if (runs++ >= PCW_CONFIG_EXPANSION_MAX_RUNS)
    THROW(pcw::Error, "Maximal number of expansion runs exeeded ",
          "(Recursion in a variable?): ", runs);
  for (auto &p : ptree) {
    for (auto &pair : p.second) {
      if (not std::regex_search(pair.second.data(), m, var))
        continue;
      auto expand = get_val(ptree, m[1]);
      auto res = std::regex_replace(pair.second.data(), var, expand,
                                    std::regex_constants::format_first_only);
      p.second.put(pair.first, res);
      goto again;
    }
  }
}

////////////////////////////////////////////////////////////////////////////////
static int get_log_level(const std::string &level) {
  if (boost::iequals(level, "debug"))
    return static_cast<int>(crow::LogLevel::Debug);
  if (boost::iequals(level, "info"))
    return static_cast<int>(crow::LogLevel::Info);
  if (boost::iequals(level, "warning"))
    return static_cast<int>(crow::LogLevel::Warning);
  if (boost::iequals(level, "error"))
    return static_cast<int>(crow::LogLevel::Error);
  if (boost::iequals(level, "critical"))
    return static_cast<int>(crow::LogLevel::Critical);
  CROW_LOG_WARNING << "(Config) Invalid log level: " << level << "; "
                   << "defaulting to info";
  return static_cast<int>(crow::LogLevel::Info);
}

////////////////////////////////////////////////////////////////////////////////
pcw::Config pcw::Config::empty() { return Config{{}, {}, {}}; }

////////////////////////////////////////////////////////////////////////////////
pcw::Config pcw::Config::load(const std::string &file) {
  std::ifstream is(file);
  if (not is.good())
    throw std::system_error(errno, std::system_category(), file);
  return load(is);
}

////////////////////////////////////////////////////////////////////////////////
pcw::Config pcw::Config::load(std::istream &is) {
  Ptree ptree;
  boost::property_tree::read_ini(is, ptree);
  expand_variables(ptree);
  auto threads = ptree.get<int>("daemon.threads");
  threads = threads < 1 ? std::thread::hardware_concurrency() : threads;

  return {
      {ptree.get<std::string>("db.user"), ptree.get<std::string>("db.host"),
       ptree.get<std::string>("db.pass"), ptree.get<std::string>("db.name"),
       ptree.get<int>("db.connections"), ptree.get<bool>("db.debug")},
      {ptree.get<std::string>("daemon.host"),
       ptree.get<std::string>("daemon.projectdir")},
      {get_log_level(ptree.get<std::string>("log.level"))},
  };
}

////////////////////////////////////////////////////////////////////////////////
void pcw::Config::setup_logging() const {
  static crow::CerrLogHandler cerrlogger;
  crow::logger::setHandler(&cerrlogger);
  crow::logger::setLogLevel(static_cast<crow::LogLevel>(log.level));
}

////////////////////////////////////////////////////////////////////////////////
void pcw::Config::LOG() const {
  CROW_LOG_DEBUG << "db.user:           " << this->db.user;
  CROW_LOG_DEBUG << "db.host:           " << this->db.host;
  CROW_LOG_DEBUG << "db.pass:           " << this->db.pass;
  CROW_LOG_DEBUG << "db.name:           " << this->db.name;
  CROW_LOG_DEBUG << "db.connections:    " << this->db.connections;
  CROW_LOG_DEBUG << "db.debug:          " << this->db.debug;

  CROW_LOG_DEBUG << "daemon.host:       " << this->daemon.host;
  CROW_LOG_DEBUG << "daemon.projectdir: " << this->daemon.projectdir;

  CROW_LOG_DEBUG << "log.level:         " << this->log.level;
}
