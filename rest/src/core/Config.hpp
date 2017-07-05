#ifndef pcw_Config_hpp__
#define pcw_Config_hpp__

#include <boost/property_tree/ptree.hpp>
#include <string>
#include <unordered_map>

namespace pcw {
using Ptree = boost::property_tree::ptree;

struct Config {
	static Config load(const std::string& filename);
	static Config load(std::istream& is);
	void setup_logging() const;

	const struct Db {
		const std::string user, host, pass, db;
		const int connections;
		const bool debug;
	} db;

	const struct Daemon {
		const std::string host, user, basedir;
		const int port, threads;
		const bool detach;
	} daemon;

	const struct Log {
		const std::string file;
		const int level;
	} log;

	const struct Plugins {
		const std::unordered_map<std::string, Ptree> configs;
		const Ptree& operator[](const std::string& p) const noexcept;
	} plugins;
	void LOG() const;
};
}

#endif  // pcw_Config_hpp__
